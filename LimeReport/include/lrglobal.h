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
#ifndef GLOBAL_H
#define GLOBAL_H
#include "qglobal.h"
#include <stdexcept>
#include <QString>

#if defined(LIMEREPORT_EXPORTS)
#  define LIMEREPORT_EXPORT Q_DECL_EXPORT
#elif defined (LIMEREPORT_IMPORTS)
#  define LIMEREPORT_EXPORT Q_DECL_IMPORT
#else
#  define LIMEREPORT_EXPORT   /**/
#endif

namespace LimeReport {

namespace Const{
    int const RESIZE_HANDLE_SIZE = 10;
    int const SELECTION_PEN_SIZE = 4;
    int const MINIMUM_ITEM_WIDTH = 2*RESIZE_HANDLE_SIZE;
    int const MINIMUM_ITEM_HEIGHT = 2*RESIZE_HANDLE_SIZE;
    double const RESIZE_ZONE_OPACITY = 0.5;
    double const SELECTED_RESIZE_ZONE_OPACITY = 0.6;
    Qt::GlobalColor const RESIZE_ZONE_COLOR = Qt::green;
    Qt::GlobalColor const SELECTION_COLOR = Qt::red;
    Qt::GlobalColor const JOIN_COLOR = Qt::blue;
    double const SELECTION_COLOR_OPACITY = 0.6;
    const qreal fontFACTOR = 3.5;
    const int mmFACTOR = 10;
    const int itemPaleteIconSize = 24;
    const qreal minSpaceBorder = 10;
    const QString bandTAG = "band";
    const Qt::GlobalColor BAND_NAME_LABEL_COLOR = Qt::yellow;
    const Qt::GlobalColor BAND_NAME_BORDER_COLOR = Qt::darkYellow;
    const qreal BAND_MARKER_OPACITY = 1;
    const qreal LAYOUT_MARKER_OPACITY = 0.3;
    const qreal BAND_NAME_AREA_OPACITY = 0.3;
    const qreal BAND_NAME_TEXT_OPACITY = 0.6;
    const qreal SELECTION_OPACITY = 0.3;
    const QString FIELD_RX = "\\$D\\s*\\{\\s*([^{}]*)\\s*\\}";
    const QString VARIABLE_RX = "\\$V\\s*\\{\\s*([^{}]*)\\s*\\}";
    const QString SCRIPT_RX = "\\$S\\s*\\{(.*)\\}";
    const QString GROUP_FUNCTION_PARAM_RX = "\\(\\s*(((?:\\\"?\\$D\\s*\\{\\s*)|(?:\\\"?\\$V\\s*\\{\\s*)|(?:\\\"))(\\w+\\.?\\w+)((?:\\\")|(?:\\s*\\}\\\"?\\s*)))\\s*,\\s*\\\"(\\w+)\\\"\\s*\\)";
    const int DATASOURCE_INDEX = 6;
    const int VALUE_INDEX = 2;
    const QString GROUP_FUNCTION_RX = "(%1\\s*"+GROUP_FUNCTION_PARAM_RX+")";
    const QString GROUP_FUNCTION_NAME_RX = "%1\\s*\\((.*[^\\)])\\)";
    const int SCENE_MARGIN = 50;
}
    QString extractClassName(QString className);
    enum RenderPass {FirstPass, SecondPass};
    enum ArrangeType {AsNeeded, Force};
    class ReportError : public std::runtime_error{
    public:
        ReportError(const QString& message):std::runtime_error(message.toStdString()){}
    };

} // namespace LimeReport



#endif // GLOBAL_H
