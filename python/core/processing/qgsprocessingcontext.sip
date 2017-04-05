/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingcontext.h                           *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/





class QgsProcessingContext
{

%TypeHeaderCode
#include "qgsprocessingcontext.h"
%End
  public:

    QgsProcessingContext();

    QgsProject *project() const;
%Docstring
 Returns the project in which the algorithm is being executed.
 \see setProject()
%End

    void setProject( QgsProject *project );
%Docstring
 Sets the project in which the algorithm will be executed.
 \see project()
%End

};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingcontext.h                           *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
