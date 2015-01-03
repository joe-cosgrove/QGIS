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

    /**Alter the brightness or contrast of a QImage.
     * @param image QImage to alter
     * @param brightness brightness value, in the range -255 to 255. A brightness value of 0 indicates
     * no change to brightness, a negative value will darken the image, and a positive value will brighten
     * the image.
     * @param contrast contrast value. Must be a positive or zero value. A value of 1.0 indicates no change
     * to the contrast, a value of 0 represents an image with 0 contrast, and a value > 1.0 will increase the
     * contrast of the image.
     */
    static void adjustBrightnessContrast( QImage &image, const int brightness, const double contrast );
    
};
