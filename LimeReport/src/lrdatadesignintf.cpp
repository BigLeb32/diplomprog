/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2015 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/
#include "lrdatadesignintf.h"

#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <stdexcept>
#include <QStringList>
#include "lrdatasourcemanager.h"

namespace LimeReport{

ModelHolder::ModelHolder(QAbstractItemModel *model, bool owned /*false*/)
{
    ModelToDataSource* mh = new ModelToDataSource(model,owned);
    m_dataSource = mh;
    m_owned=owned;
    connect(mh, SIGNAL(modelStateChanged()), this, SIGNAL(modelStateChanged()));
}

ModelHolder::~ModelHolder(){
    delete m_dataSource;
}

IDataSource * ModelHolder::dataSource(IDataSource::DatasourceMode mode)
{
    Q_UNUSED(mode);
    return m_dataSource;
}

QueryHolder::QueryHolder(QString queryText, QString connectionName, DataSourceManager *dataManager)
    : m_query(0), m_queryText(queryText), m_connectionName(connectionName),
      m_mode(IDataSource::RENDER_MODE), m_dataManager(dataManager), m_prepared(true)
{
    extractParams();
}

QueryHolder::~QueryHolder()
{
    if (m_query) delete m_query;
}

bool QueryHolder::runQuery(IDataSource::DatasourceMode mode)
{
    m_mode = mode;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isValid()) {
        setLastError(QObject::tr("Invalid connection! %1").arg(m_connectionName));
        return false;
    }

    if (!m_prepared){
        extractParams();
        if (!m_prepared) return false;
    }

    if (!m_query){
        m_query = new QSqlQuery(db);
        m_query->prepare(m_preparedSQL);
    }

    fillParams(m_query);
    m_query->exec();

    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery(*m_query);

    while (model->canFetchMore())
        model->fetchMore();

    if (model->lastError().isValid()){
        if (m_dataSource)
           m_dataSource.clear();
        setLastError(model->lastError().text());
        delete model;
        return false;
    } else setLastError("");

    setDatasource(IDataSource::Ptr(new ModelToDataSource(model,true)));
    return true;
}

QString QueryHolder::connectionName()
{
    return m_connectionName;
}

void QueryHolder::setConnectionName(QString connectionName)
{
    m_connectionName=connectionName;
}

void QueryHolder::invalidate(IDataSource::DatasourceMode mode){
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    if (!db.isValid()){
        setLastError(QObject::tr("Invalid connection! %1").arg(m_connectionName));
        delete m_query;
        m_dataSource.clear();
    } else {
        runQuery(mode);
    }

}

void QueryHolder::update()
{
    runQuery(m_mode);
}

void QueryHolder::setDatasource(IDataSource::Ptr value){
    m_dataSource.clear();
    m_dataSource=value;
}

void QueryHolder::fillParams(QSqlQuery *query)
{
    foreach(QString param,m_aliasesToParam.keys()){
        QVariant value;
        if (param.contains(".")){
            value = dataManager()->fieldData(m_aliasesToParam.value(param));
            param=param.right(param.length()-param.indexOf('.')-1);
        } else {
            value = dataManager()->variable(m_aliasesToParam.value(param));
        }
        if (value.isValid() || m_mode == IDataSource::DESIGN_MODE)
            query->bindValue(':'+param,value);
    }
}

void QueryHolder::extractParams()
{
    m_preparedSQL = replaceVariables(m_queryText);
}

QString QueryHolder::replaceVariables(QString query)
{
    QRegExp rx(Const::VARIABLE_RX);
    int curentAliasIndex = 0;
    if (query.contains(rx)){
        int pos = -1;
        while ((pos=rx.indexIn(query))!=-1){

            QString variable=rx.cap(0);
            variable.remove("$V{");
            variable.remove("}");

            if (m_aliasesToParam.contains(variable)){
                curentAliasIndex++;
                m_aliasesToParam.insert(variable+"_alias"+QString::number(curentAliasIndex),variable);
                variable += "_alias"+QString::number(curentAliasIndex);
            } else {
                m_aliasesToParam.insert(variable,variable);
            }

            query.replace(pos,rx.cap(0).length(),":"+variable);

        }
    }
    return query;
}

QString QueryHolder::queryText()
{
    return m_queryText;
}

void QueryHolder::setQueryText(QString queryText)
{
    m_queryText=queryText;
}

IDataSource* QueryHolder::dataSource(IDataSource::DatasourceMode mode)
{
    if ((m_mode != mode && m_mode == IDataSource::DESIGN_MODE) || m_dataSource==0) {
        m_mode = mode;
        runQuery(mode);
    }
    if (m_dataSource)
        return m_dataSource.data();
    else return 0;
}

// QueryHolder

// ModelToDataSource

ModelToDataSource::ModelToDataSource(QAbstractItemModel* model, bool owned)
    : QObject(), m_model(model), m_owned(owned), m_curRow(-1), m_lastError("")
{
    Q_ASSERT(model);
    if (model){
        while (model->canFetchMore(QModelIndex()))
            model->fetchMore(QModelIndex());
        connect(model, SIGNAL(destroyed()), this, SLOT(slotModelDestroed()));
        connect(model, SIGNAL(modelReset()), this, SIGNAL(modelStateChanged()));
    }
}

ModelToDataSource::~ModelToDataSource()
{
    if ((m_owned) && m_model!=0)
        delete m_model;
}

bool ModelToDataSource::next()
{
    if (isInvalid()) return false;
    if (m_curRow<(m_model->rowCount())) {
        if (bof()) m_curRow++;
        m_curRow++;
        return true;
    } else return false;
}

bool ModelToDataSource::hasNext()
{
    if (isInvalid()) return false;
    return m_curRow<m_model->rowCount()-1;
}

bool ModelToDataSource::prior()
{
    if (isInvalid()) return false;
    if (m_curRow>-1){
        if (eof()) m_curRow--;
        m_curRow--;
        return true;
    } else return false;
}

void ModelToDataSource::first()
{
    m_curRow=0;
}

void ModelToDataSource::last()
{
    if (isInvalid()) m_curRow=0;
    else m_curRow=m_model->rowCount()-1;
}

bool ModelToDataSource::eof()
{
    if (isInvalid()) return true;
    return (m_curRow==m_model->rowCount())||(m_model->rowCount()==0);
}

bool ModelToDataSource::bof()
{
    if (isInvalid()) return true;
    return (m_curRow==-1)||(m_model->rowCount()==0);
}

QVariant ModelToDataSource::data(const QString &columnName)
{
    if (isInvalid()) return QVariant();
    return m_model->data(m_model->index(currentRow(),columnIndexByName(columnName)));
}

int ModelToDataSource::columnCount()
{
    if (isInvalid()) return 0;
    return m_model->columnCount();
}

QString ModelToDataSource::columnNameByIndex(int columnIndex)
{
    if (isInvalid()) return "";
    return m_model->headerData(columnIndex,Qt::Horizontal).toString();
}

int ModelToDataSource::columnIndexByName(QString name)
{
    if (isInvalid()) return 0;
    for(int i=0;i<m_model->columnCount();i++){
        if (m_model->headerData(i,Qt::Horizontal).toString().compare(name,Qt::CaseInsensitive)==0)
            return i;
    }
    return -1;
}

QString ModelToDataSource::lastError()
{
    return m_lastError;
}

QAbstractItemModel * ModelToDataSource::model()
{
    return m_model;
}

int ModelToDataSource::currentRow()
{
    if (eof()) return m_curRow-1;
    if (bof()) return m_curRow+1;
    return m_curRow;
}

bool ModelToDataSource::isInvalid() const
{
    return m_model==0;
}

void ModelToDataSource::slotModelDestroed()
{
    m_model = 0;
    m_lastError = tr("model is destroyed");
    emit modelStateChanged();
}

ConnectionDesc::ConnectionDesc(QSqlDatabase db, QObject *parent)
    : QObject(parent), m_connectionName(db.connectionName()), m_connectionHost(db.hostName()), m_connectionDriver(db.driverName()),
      m_databaseName(db.databaseName()), m_user(db.userName()), m_password(db.password()), m_autoconnect(false)
{}

ConnectionDesc::ConnectionDesc(QObject *parent)
    :QObject(parent),m_connectionName(""),m_connectionHost(""),m_connectionDriver(""),
      m_databaseName(""), m_user(""), m_password(""), m_autoconnect(false)
{}

ConnectionDesc::Ptr ConnectionDesc::create(QSqlDatabase db, QObject *parent)
{
    return Ptr(new ConnectionDesc(db,parent));
}

void ConnectionDesc::setName(const QString &value)
{
    if (m_connectionName!=value) emit nameChanged(m_connectionName,value);
    m_connectionName=value;
}

QueryDesc::QueryDesc(QString queryName, QString queryText, QString connection)
    :m_queryName(queryName), m_query(queryText), m_connectionName(connection)
{}

SubQueryHolder::SubQueryHolder(QString queryText, QString connectionName, QString masterDatasource, DataSourceManager* dataManager)
    : QueryHolder(queryText, connectionName, dataManager), m_masterDatasource(masterDatasource)/*, m_invalid(false)*/
{
    extractParams();
}

void SubQueryHolder::setMasterDatasource(const QString &value)
{
    if (dataManager()->dataSource(value)){
        m_masterDatasource = value;
    }
}

void SubQueryHolder::extractParams()
{
    if (!dataManager()->containsDatasource(m_masterDatasource)){
        setLastError(QObject::tr("Master datasource \"%1\" not found!!!").arg(m_masterDatasource));
        setPrepared(false);
    } else {
        m_preparedSQL = replaceFields(replaceVariables(queryText()));
        setPrepared(true);
    }
}

QString SubQueryHolder::extractField(QString source)
{
    if (source.contains('.')) {
        return source.right(source.length()-(source.indexOf('.')+1));
    }
    return source;
}

QString SubQueryHolder::replaceFields(QString query)
{
    QRegExp rx(Const::FIELD_RX);
    int curentAliasIndex=0;
    if (query.contains(rx)){
        int pos;
        while ((pos=rx.indexIn(query))!=-1){
            QString field=rx.cap(0);
            field.remove("$D{");
            field.remove("}");

            if (!m_aliasesToParam.contains(field)){
                if (field.contains("."))
                    m_aliasesToParam.insert(field,field);
                else
                    m_aliasesToParam.insert(field,m_masterDatasource+"."+field);
            } else {
                curentAliasIndex++;
                if (field.contains("."))
                    m_aliasesToParam.insert(field+"_alias"+QString::number(curentAliasIndex),field);
                else
                    m_aliasesToParam.insert(field+"_alias"+QString::number(curentAliasIndex),m_masterDatasource+"."+field);
                field+="_alias"+QString::number(curentAliasIndex);
            }
            query.replace(pos,rx.cap(0).length(),":"+extractField(field));
        }
    }
    return query;
}

SubQueryDesc::SubQueryDesc(QString queryName, QString queryText, QString connection, QString masterDatasourceName)
    :QueryDesc(queryName,queryText,connection), m_masterDatasourceName(masterDatasourceName)
{
}

QObject *ProxyDesc::createElement(const QString &collectionName, const QString &)
{
    if (collectionName=="fields"){
        FieldMapDesc* fieldMapDesc = new FieldMapDesc;
        m_maps.append(fieldMapDesc);
        return fieldMapDesc;
    }
    return 0;
}

int ProxyDesc::elementsCount(const QString &collectionName)
{
    Q_UNUSED(collectionName)
    return m_maps.count();
}

QObject *ProxyDesc::elementAt(const QString &collectionName, int index)
{
    Q_UNUSED(collectionName)
    return m_maps.at(index);
}

ProxyHolder::ProxyHolder(ProxyDesc* desc, DataSourceManager* dataManager)
    :m_model(0), m_desc(desc), m_lastError(""), m_mode(IDataSource::RENDER_MODE),
     m_invalid(false), m_dataManger(dataManager)
{}

QString ProxyHolder::masterDatasource()
{
    if (m_desc) return m_desc->master();
    return QString();
}

void ProxyHolder::filterModel()
{
    if (!m_datasource){

        if (dataManager()){
            IDataSource* master = dataManager()->dataSource(m_desc->master());
            IDataSource* child = dataManager()->dataSource(m_desc->child());
            if (master&&child){
                m_model = new MasterDetailProxyModel(dataManager());
                connect(child->model(),SIGNAL(destroyed()), this, SLOT(slotChildModelDestoroyed()));
                m_model->setSourceModel(child->model());
                m_model->setMaster(m_desc->master());
                m_model->setChildName(m_desc->child());
                m_model->setFieldsMap(m_desc->fieldsMap());
                try{
                    m_model->rowCount();
                    m_datasource = IDataSource::Ptr(new ModelToDataSource(m_model,true));
                } catch (ReportError& exception) {
                    m_lastError = exception.what();
                }
                m_invalid = false;
                m_lastError.clear();
            } else {
                m_lastError.clear();
                if(!master) m_lastError+=QObject::tr("Master datasouce \"%1\" not found!").arg(m_desc->master());
                if(!child) m_lastError+=((m_lastError.isEmpty())?QObject::tr("Child"):QObject::tr(" and child "))+
                                          QObject::tr("datasouce \"%1\" not found!").arg(m_desc->child());
            }
        }
    } else {
        if (!isInvalid()){
            m_model->invalidate();
            m_datasource->first();
        }
    }
}

IDataSource *ProxyHolder::dataSource(IDataSource::DatasourceMode mode)
{
    if ((m_mode != mode && m_mode == IDataSource::DESIGN_MODE) || m_datasource==0) {
        m_mode = mode;
        m_datasource.clear();
        filterModel();
    }
    return m_datasource.data();
}

void ProxyHolder::invalidate(IDataSource::DatasourceMode mode)
{
    Q_UNUSED(mode)
    if (m_model && m_model->isInvalid()){
        m_invalid = true;
        m_lastError = tr("Datasource has been invalidated");
    } else {
        filterModel();
    }
}

void ProxyHolder::slotChildModelDestoroyed(){
    m_datasource.clear();
    m_model = 0;
}

void ProxyDesc::addFieldsCorrelation(const FieldsCorrelation& fieldsCorrelation)
{
    m_maps.append(new FieldMapDesc(fieldsCorrelation));
}

void MasterDetailProxyModel::setMaster(QString name){
    m_masterName=name;
}

bool MasterDetailProxyModel::isInvalid() const
{
    if (m_masterName.isEmpty() || m_childName.isEmpty()) return true;
    IDataSource* masterData = dataManager()->dataSource(m_masterName);
    IDataSource* childData = dataManager()->dataSource(m_childName);
    if (!masterData || !childData) return true;
    return masterData->isInvalid() || childData->isInvalid();
}

bool MasterDetailProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    foreach (FieldMapDesc* fieldCorrelation, *m_maps) {
        QVariant master = masterData(fieldCorrelation->master());
        QVariant detail = sourceData(fieldCorrelation->detail(),source_row);
        if (master==detail)
            return true;
    }
    return false;
}

int MasterDetailProxyModel::fieldIndexByName(QString fieldName) const
{
    for(int i=0;i<sourceModel()->columnCount();++i){
        if (sourceModel()->headerData(i,Qt::Horizontal).toString().compare(fieldName,Qt::CaseInsensitive)==0){
           return i;
        }
    }
    return -1;
}

QVariant MasterDetailProxyModel::sourceData(QString fieldName, int row) const
{
    int fieldIndex = fieldIndexByName(fieldName);
    if (fieldIndex!=-1){
        return sourceModel()->index(row,fieldIndex).data();
    } else {
        throw ReportError(
            tr("Field: \"%1\" not found in \"%2\" child datasource").arg(fieldName).arg(m_childName)
        );
    }
}

QVariant MasterDetailProxyModel::masterData(QString fieldName) const
{
    IDataSource* master = dataManager()->dataSource(m_masterName);
    int columnIndex = master->columnIndexByName(fieldName);
    if (columnIndex!=-1){
        return master->data(fieldName);
    } else {
        throw ReportError(
            tr("Field: \"%1\" not found in \"%2\" master datasource").arg(fieldName).arg(m_masterName)
        );
    }
}

bool CallbackDatasource::next(){
    if (!m_eof){
        m_currentRow++;
        bool result = false;
        emit changePos(CallbackInfo::Next,result);
        m_eof = !result; // !checkNextRecord(m_currentRow);
        return result;
    } else return false;
}

void CallbackDatasource::first(){
    m_currentRow = 0;
    m_eof=checkIfEmpty();
    bool result=false;
    emit changePos(CallbackInfo::First,result);
    m_eof = !result;
}

QVariant CallbackDatasource::data(const QString& columnName)
{
    CallbackInfo info;
    info.dataType = CallbackInfo::ColumnData;
    info.columnName = columnName;
    info.index = m_currentRow;
    QVariant result;
    emit getCallbackData(info,result);
    return result;
}

int CallbackDatasource::columnCount(){
    if (m_headers.size()==0){
        int currIndex = 0;
        do {
            QVariant columnName;
            CallbackInfo info;
            info.dataType = CallbackInfo::ColumnHeaderData;
            info.index = currIndex;
            emit getCallbackData(info,columnName);
            if (columnName.isValid()){
                m_headers.append(columnName.toString());
                currIndex++;
            } else break;
        } while (true);
    }
    return m_headers.size();
}

QString CallbackDatasource::columnNameByIndex(int columnIndex)
{
    if (columnIndex < m_headers.size())
        return m_headers[columnIndex];
    else return QString();
}

int CallbackDatasource::columnIndexByName(QString name)
{
    for (int i=0;i<m_headers.size();++i) {
        if (m_headers[i].compare(name) == 0)
            return i;
    }
//    if (m_headers.size()==0){
//        QVariant data;
//        bool hasNextRow;
//        emit getData(m_currentRow, name, data, hasNextRow);
//        if (data.isValid()) return 0;
//    }
    return -1;
}

} //namespace LimeReport
