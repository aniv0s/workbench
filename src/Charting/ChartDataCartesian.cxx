
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __CHART_DATA_CARTESIAN_DECLARE__
#include "ChartDataCartesian.h"
#undef __CHART_DATA_CARTESIAN_DECLARE__

#include <limits>

#include <QTextStream>

#include "CaretAssert.h"
#include "ChartPoint.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartDataCartesian 
 * \brief Chart cartesian data.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *   Type of chart data model.
 * @param dataAxisUnitsX
 *   Data units for X-axis.
 * @param dataAxisUnitsY
 *   Data units for Y-axis.
 */
ChartDataCartesian::ChartDataCartesian(const ChartDataTypeEnum::Enum chartDataType,
                                                 const ChartAxisUnitsEnum::Enum dataAxisUnitsX,
                                                 const ChartAxisUnitsEnum::Enum dataAxisUnitsY)
: ChartData(chartDataType),
m_dataAxisUnitsX(dataAxisUnitsX),
m_dataAxisUnitsY(dataAxisUnitsY)
{
    initializeMembersChartDataCartesian();
}

/**
 * Destructor.
 */
ChartDataCartesian::~ChartDataCartesian()
{
    removeAllPoints();
    
    delete m_sceneAssistant;
}

/**
 * Initialize members of a new instance.
 */
void
ChartDataCartesian::initializeMembersChartDataCartesian()
{
    m_boundsValid       = false;
    m_color             = CaretColorEnum::RED;
    m_timeStartInSecondsAxisX = 0.0;
    m_timeStepInSecondsAxisX  = 1.0;
    
    std::vector<CaretColorEnum::Enum> colorEnums;
    CaretColorEnum::getAllEnums(colorEnums);
    const int32_t numCaretColors = static_cast<int32_t>(colorEnums.size());
    
    bool colorFound = false;
    while ( ! colorFound) {
        ChartDataCartesian::caretColorIndex++;
        if (ChartDataCartesian::caretColorIndex >= numCaretColors) {
            ChartDataCartesian::caretColorIndex = 0;
        }
        
        if (colorEnums[ChartDataCartesian::caretColorIndex] == CaretColorEnum::BLACK) {
            /* do not use black */
        }
        else if (colorEnums[ChartDataCartesian::caretColorIndex] == CaretColorEnum::WHITE) {
            /* do not use white */
        }
        else {
            m_color = colorEnums[ChartDataCartesian::caretColorIndex];
            colorFound = true;
        }
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add<ChartAxisUnitsEnum, ChartAxisUnitsEnum::Enum>("m_dataAxisUnitsX",
                                                                &m_dataAxisUnitsX);
    m_sceneAssistant->add<ChartAxisUnitsEnum, ChartAxisUnitsEnum::Enum>("m_dataAxisUnitsY",
                                                                &m_dataAxisUnitsY);
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_color",
                                                                &m_color);
    m_sceneAssistant->add("m_timeStartInSecondsAxisX",
                          &m_timeStartInSecondsAxisX);
    m_sceneAssistant->add("m_timeStepInSecondsAxisX",
                          &m_timeStepInSecondsAxisX);
}

/**
 * Remove all points in the model.
 */
void
ChartDataCartesian::removeAllPoints()
{
    for (std::vector<ChartPoint*>::const_iterator iter = m_points.begin();
         iter != m_points.end();
         iter++) {
        delete *iter;
    }
    m_points.clear();
    
    m_boundsValid = false;
}

/**
 * At times a copy of chart data will be needed BUT it must be
 * the proper subclass so copy constructor and assignment operator
 * will no function when this abstract, base class is used.  Each
 * subclass will override this method so that the returned class
 * is of the proper type.
 *
 * @return Copy of this instance that is the actual subclass.
 */
ChartData*
ChartDataCartesian::clone() const
{
    ChartDataCartesian* cloneCopy = new ChartDataCartesian(*this);
    return cloneCopy;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartDataCartesian::ChartDataCartesian(const ChartDataCartesian& obj)
: ChartData(obj),
m_dataAxisUnitsX(obj.m_dataAxisUnitsX),
m_dataAxisUnitsY(obj.m_dataAxisUnitsY)
{
    initializeMembersChartDataCartesian();
    this->copyHelperChartDataCartesian(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartDataCartesian&
ChartDataCartesian::operator=(const ChartDataCartesian& obj)
{
    if (this != &obj) {
        ChartData::operator=(obj);
        this->copyHelperChartDataCartesian(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartDataCartesian::copyHelperChartDataCartesian(const ChartDataCartesian& obj)
{
    m_dataAxisUnitsX = obj.m_dataAxisUnitsX;
    m_dataAxisUnitsY = obj.m_dataAxisUnitsY;
    
    removeAllPoints();

    for (std::vector<ChartPoint*>::const_iterator iter = obj.m_points.begin();
         iter != obj.m_points.end();
         iter++) {
        const ChartPoint* cp = *iter;
        m_points.push_back(new ChartPoint(*cp));
    }

    m_boundsValid       = false;
    m_color             = obj.m_color;
    m_timeStartInSecondsAxisX = obj.m_timeStartInSecondsAxisX;
    m_timeStepInSecondsAxisX  = obj.m_timeStepInSecondsAxisX;
}

/**
 * Add a point.
 *
 * @param x
 *    X-coordinate.
 * @param y
 *    Y-coordinate.
 */
void
ChartDataCartesian::addPoint(const float x,
                                  const float y)
{
    m_points.push_back(new ChartPoint(x, y));
    m_boundsValid = false;
}

/**
 * @return Number of points.
 */
int32_t
ChartDataCartesian::getNumberOfPoints() const
{
    return m_points.size();
}

/**
 * Get the point at the given index.
 *
 * @param pointIndex
 *    Index of point.
 * @return
 *    Point at the given index.
 */
const ChartPoint*
ChartDataCartesian::getPointAtIndex(const int32_t pointIndex) const
{
    CaretAssertVectorIndex(m_points, pointIndex);
    return m_points[pointIndex];
}

/**
 * Get the bounds of all of the points.
 *
 * @param xMinimumOut
 *     Minimum X-coordinate of all points.
 * @param xMaximumOut
 *     Maximum X-coordinate of all points.
 * @param yMinimumOut
 *     Minimum Y-coordinate of all points.
 * @param yMaximumOut
 *     Maximum Y-coordinate of all points.
 */
void
ChartDataCartesian::getBounds(float& xMinimumOut,
                                   float& xMaximumOut,
                                   float& yMinimumOut,
                                   float& yMaximumOut) const
{
    if (! m_boundsValid) {
        float xMin = 0.0;
        float xMax = 0.0;
        float yMin = 0.0;
        float yMax = 0.0;
        float zMin = 0.0;
        float zMax = 0.0;
        const int32_t numPoints = getNumberOfPoints();
        if (numPoints > 0) {
            xMin = std::numeric_limits<float>::max();
            xMax = -std::numeric_limits<float>::max();
            yMin = std::numeric_limits<float>::max();
            yMax = -std::numeric_limits<float>::max();
            for (int32_t i = 0; i < numPoints; i++) {
                const float* xy = getPointAtIndex(i)->getXY();
                const float x = xy[0];
                const float y = xy[1];
                if (x < xMin) xMin = x;
                if (x > xMax) xMax = x;
                if (y < yMin) yMin = y;
                if (y > yMax) yMax = y;
            }
            
            m_boundsValid = true;
        }
        
        m_bounds[0] = xMin;
        m_bounds[1] = xMax;
        m_bounds[2] = yMin;
        m_bounds[3] = yMax;
        m_bounds[4] = zMin;
        m_bounds[5] = zMax;
    }
    
    xMinimumOut = m_bounds[0];
    xMaximumOut = m_bounds[1];
    yMinimumOut = m_bounds[2];
    yMaximumOut = m_bounds[3];
}

/**
 * @return The time start in seconds for the X-Axis (Valid when
 * the X-axis is time)
 */
float
ChartDataCartesian::getTimeStartInSecondsAxisX() const
{
    return m_timeStartInSecondsAxisX;
}

/**
 * Set the time start in seconds for the X-Axis (Valid when
 * the X-axis is time)
 *
 * @param timeStartInSecondsAxisX
 *     Time of first point in the X-axis.
 */
void
ChartDataCartesian::setTimeStartInSecondsAxisX(const float timeStartInSecondsAxisX)
{
    m_timeStartInSecondsAxisX = timeStartInSecondsAxisX;
}

/**
 * @return The time step in seconds for the X-Axis (Valid when
 * the X-axis is time)
 */
float
ChartDataCartesian::getTimeStepInSecondsAxisX() const
{
    return m_timeStepInSecondsAxisX;
}

/**
 * Set the time step in seconds for the X-Axis (Valid when
 * the X-axis is time)
 *
 * @param timeStepInSecondsAxisX
 *     Number of seconds between consecutive points in X-axis.
 */
void
ChartDataCartesian::setTimeStepInSecondsAxisX(const float timeStepInSecondsAxisX)
{
    m_timeStepInSecondsAxisX = timeStepInSecondsAxisX;
}

/**
 * @return Data units for X axis
 */
ChartAxisUnitsEnum::Enum
ChartDataCartesian::getDataAxisUnitsX()
{
    return m_dataAxisUnitsX;
}

/**
 * @return Data units for Y axis
 */
ChartAxisUnitsEnum::Enum
ChartDataCartesian::getDataAxisUnitsY()
{
    return m_dataAxisUnitsY;
}

/**
 * @return Color for chart
 */
CaretColorEnum::Enum
ChartDataCartesian::getColor() const
{
    return m_color;
}

/**
 * Save subclass data to the scene.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
ChartDataCartesian::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                     SceneClass* sceneClass)
{
    SceneClass* chartDataCartesian = new SceneClass("chartDataCartesian",
                                               "ChartDataCartesian",
                                               1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  chartDataCartesian);
    
    const int32_t numPoints2D = getNumberOfPoints();
    if (numPoints2D > 0) {
        chartDataCartesian->addInteger("numberOfPoints2D",
                                       numPoints2D);
        
        AString pointString;
        pointString.reserve(numPoints2D * 2 * 10);
        QTextStream textStream(&pointString,
                               QIODevice::WriteOnly);
        
        for (int32_t i = 0; i < numPoints2D; i++) {
            const float* xy = m_points[i]->getXY();
            textStream << xy[0] << " " << xy[1] << " ";
        }
        
        chartDataCartesian->addString("points2D",
                                      pointString);
    }
    
    sceneClass->addClass(chartDataCartesian);
}

/**
 * Restore file data from the scene.  The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
ChartDataCartesian::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass)
{
    removeAllPoints();
    
    const SceneClass* chartDataCartesian = sceneClass->getClass("chartDataCartesian");
    if (chartDataCartesian == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, chartDataCartesian);
    
    const int32_t numPoints2D = chartDataCartesian->getIntegerValue("numberOfPoints2D",
                                                                  -1);
    
    if (numPoints2D > 0) {
        AString pointString = chartDataCartesian->getStringValue("points2D",
                                                                       "");
        if ( ! pointString.isEmpty()) {
            float x, y;
            QTextStream textStream(&pointString,
                                   QIODevice::ReadOnly);
            for (int32_t i = 0; i < numPoints2D; i++) {
                if (textStream.atEnd()) {
                    sceneAttributes->addToErrorMessage("Tried to read "
                                                       + AString::number(numPoints2D)
                                                       + " but only got "
                                                       + AString::number(i));
                    break;
                }
                
                textStream >> x;
                textStream >> y;
                m_points.push_back(new ChartPoint(x, y));
            }
        }
    }
}
