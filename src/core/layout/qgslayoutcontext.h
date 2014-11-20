/***************************************************************************
                             qgslayoutcontext.h
                             -------------------
    begin                : November 2014
    copyright            : (C) 2014 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSLAYOUTCONTEXT_H
#define QGSLAYOUTCONTEXT_H

#include <QObject>

class QgsFeature;
class QgsVectorLayer;

/**\ingroup Layout
 * \class QgsLayoutContext
 * \brief
 * \note added in QGIS 2.7
 */
class CORE_EXPORT QgsLayoutContext: public QObject
{
    Q_OBJECT

  public:

    /**Flags for controlling how a layout is rendered
     */
    enum Flag
    {
      Debug       = 0x01,  /*< debug/testing mode, items are drawn as solid rectangles */
      OutlineOnly    = 0x02, /*< render items as outlines only */
      Antialiasing  = 0x04, /*< use antialiasing when drawing items  */
      UseAdvancedEffects = 0x08 /*< enable advanced effects such as blend modes */
    };
    Q_DECLARE_FLAGS( Flags, Flag )

    QgsLayoutContext();

    virtual ~QgsLayoutContext();

    /**Set combination of flags that will be used for rendering the layout
     * @param flags rendering flags
     * @see setFlag
     * @see flags
     * @see testFlag
     */
    void setFlags( const QgsLayoutContext::Flags flags );

    /**Enable or disable a particular rendering flag for the layout. Other existing
     * flags are not affected.
     * @param flag rendering flag to set
     * @param on set to true to enable flag
     * @see setFlags
     * @see flags
     * @see testFlag
     */
    void setFlag( const Flag flag, const bool on = true );

    /**Returns the current combination of flags used for rendering the layout.
     * @returns layout rendering flags
     * @see setFlags
     * @see setFlag
     * @see testFlag
     */
    Flags flags() const;

    /**Check whether a particular rendering flag is enabled for the layout.
     * @param flag flag to test
     * @returns true if flag is enabled
     * @see setFlags
     * @see setFlag
     * @see flags
     */
    bool testFlag( const Flag flag ) const;

    /**Sets the current feature for evaluating the layout. This feature may
     * be used for altering an item's content and appearance for a report
     * or atlas layout.
     * @param feature feature for layout. A copy of the feature is made and stored
     * in the layout context.
     * @see feature
     * @see featureChanged
     */
    void setFeature( const QgsFeature* feature );

    /**Returns the current feature for evaluating the layout. This feature may
     * be used for altering an item's content and appearance for a report
     * or atlas layout.
     * @returns current feature for layout
     * @see setFeature
     * @see featureChanged
     * @note when assigning a feature to a layout context, a copy of the feature is
     * created by setFeature. Accordingly, the feature returned by this method
     * will be a copy of the original assigned feature.
     */
    QgsFeature* feature() const { return mFeature; }

    /**Returns the vector layer associated with the layout's context.
     * @returns associated vector layer
     * @see setLayer
     */
    QgsVectorLayer* layer() const { return mLayer; }

    /**Sets the vector layer associated with the layout's context.
     * @param layer associated vector layer
     * @see layer
     */
    void setLayer( QgsVectorLayer* layer );

  signals:

    /**Emitted when the current feature changes for the layout context
     * @param feature new feature
     * @see feature
     * @see setFeature
     * @note when assigning a feature to a layout context, a copy of the feature is
     * created by setFeature. Accordingly, the feature passed by this method
     * will be a copy of the original assigned feature.
     */
    void featureChanged( QgsFeature* feature );

    /**Emitted when the context's associated vector layer changes.
     * @param layer new layer
     * @see layer
     * @see setLayer
     */
    void layerChanged( QgsVectorLayer* layer );

  private:

    Flags mFlags;

    QgsFeature* mFeature;
    QgsVectorLayer* mLayer;

};

#endif //QGSLAYOUTCONTEXT_H



