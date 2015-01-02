/** \ingroup core
 * \class QgsImageOperation
 * \brief Contains operations and filters which apply to QImages
 *
 * A set of optimised pixel manipulation operations and filters which can be applied
 * to QImages.
 * \note Added in version 2.7
 */
class QgsImageOperation
{
%TypeHeaderCode
#include <qgsimageoperation.h>
%End

  public:

    /** Modes for converting a QImage to grayscale
    */
    enum GrayscaleMode
    {
      GrayscaleLightness, /*< keep the lightness of the color, drops the saturation */
      GrayscaleLuminosity, /*< grayscale by perceptual luminosity (weighted sum of color RGB components) */
      GrayscaleAverage /*< grayscale by taking average of color RGB components */
    };

    /**Convert a QImage to a grayscale image. Alpha channel is preserved.
     * @param image QImage to convert
     * @param mode mode to use during grayscale conversion
    */
    static void convertToGrayscale( QImage &image, const GrayscaleMode mode = GrayscaleLuminosity );

};
