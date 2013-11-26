/***********************************************************************************
 *                                                                                 *
 * Voreen - The Volume Rendering Engine                                            *
 *                                                                                 *
 * Copyright (C) 2005-2013 University of Muenster, Germany.                        *
 * Visualization and Computer Graphics Group <http://viscg.uni-muenster.de>        *
 * For a list of authors please refer to the file "CREDITS.txt".                   *
 *                                                                                 *
 * This file is part of the Voreen software package. Voreen is free software:      *
 * you can redistribute it and/or modify it under the terms of the GNU General     *
 * Public License version 2 as published by the Free Software Foundation.          *
 *                                                                                 *
 * Voreen is distributed in the hope that it will be useful, but WITHOUT ANY       *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR   *
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.      *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License in the file   *
 * "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>. *
 *                                                                                 *
 * For non-commercial academic use see the license exception specified in the file *
 * "LICENSE-academic.txt". To get information about commercial licensing please    *
 * contact the authors.                                                            *
 *                                                                                 *
 ***********************************************************************************/

#include "itk_generatedmodule.h"

#include "processors/itk_AnisotropicSmoothing/curvatureanisotropicdiffusionimagefilter.h"
#include "processors/itk_AnisotropicSmoothing/gradientanisotropicdiffusionimagefilter.h"
#include "processors/itk_AnisotropicSmoothing/vectorcurvatureanisotropicdiffusionimagefilter.h"
#include "processors/itk_AnisotropicSmoothing/vectorgradientanisotropicdiffusionimagefilter.h"
#include "processors/itk_AntiAlias/antialiasbinaryimagefilter.h"
#include "processors/itk_BinaryMathematicalMorphology/binarydilateimagefilter.h"
#include "processors/itk_BinaryMathematicalMorphology/binaryerodeimagefilter.h"
#include "processors/itk_BinaryMathematicalMorphology/binarymorphologicalclosingimagefilter.h"
#include "processors/itk_BinaryMathematicalMorphology/binarymorphologicalopeningimagefilter.h"
#include "processors/itk_BinaryMathematicalMorphology/binarypruningimagefilter.h"
#include "processors/itk_BinaryMathematicalMorphology/dilateobjectmorphologyimagefilter.h"
#include "processors/itk_BinaryMathematicalMorphology/erodeobjectmorphologyimagefilter.h"
#include "processors/itk_ConnectedComponents/connectedcomponentimagefilter.h"
#include "processors/itk_ConnectedComponents/hardconnectedcomponentimagefilter.h"
#include "processors/itk_ConnectedComponents/relabelcomponentimagefilter.h"
#include "processors/itk_ConnectedComponents/scalarconnectedcomponentimagefilter.h"
#include "processors/itk_ConnectedComponents/thresholdmaximumconnectedcomponentsimagefilter.h"
#include "processors/itk_CurvatureFlow/curvatureflowimagefilter.h"
#include "processors/itk_CurvatureFlow/minmaxcurvatureflowimagefilter.h"
#include "processors/itk_DistanceMap/contourdirectedmeandistanceimagefilter.h"
#include "processors/itk_DistanceMap/contourmeandistanceimagefilter.h"
#include "processors/itk_DistanceMap/danielssondistancemapimagefilter.h"
#include "processors/itk_DistanceMap/directedhausdorffdistanceimagefilter.h"
#include "processors/itk_DistanceMap/hausdorffdistanceimagefilter.h"
#include "processors/itk_DistanceMap/signeddanielssondistancemapimagefilter.h"
#include "processors/itk_ImageCompare/absolutevaluedifferenceimagefilter.h"
#include "processors/itk_ImageCompare/checkerboardimagefilter.h"
#include "processors/itk_ImageCompare/similarityindeximagefilter.h"
#include "processors/itk_ImageCompare/squareddifferenceimagefilter.h"
#include "processors/itk_ImageFeature/cannyedgedetectionimagefilter.h"
#include "processors/itk_ImageFeature/derivativeimagefilter.h"
#include "processors/itk_ImageFeature/laplacianimagefilter.h"
#include "processors/itk_ImageFeature/laplacianrecursivegaussianimagefilter.h"
#include "processors/itk_ImageFeature/laplaciansharpeningimagefilter.h"
#include "processors/itk_ImageFeature/simplecontourextractorimagefilter.h"
#include "processors/itk_ImageFeature/sobeledgedetectionimagefilter.h"
#include "processors/itk_ImageFeature/zerocrossingbasededgedetectionimagefilter.h"
#include "processors/itk_ImageFeature/zerocrossingimagefilter.h"
#include "processors/itk_ImageFusion/labeloverlayimagefilter.h"
#include "processors/itk_ImageFusion/labeltorgbimagefilter.h"
#include "processors/itk_ImageGradient/gradientimagefilter.h"
#include "processors/itk_ImageGradient/gradientmagnitudeimagefilter.h"
#include "processors/itk_ImageGradient/gradientmagnituderecursivegaussianimagefilter.h"
#include "processors/itk_ImageGradient/gradientrecursivegaussianimagefilter.h"
#include "processors/itk_ImageGradient/vectorgradientmagnitudeimagefilter.h"
#include "processors/itk_ImageIntensity/absimagefilter.h"
#include "processors/itk_ImageIntensity/acosimagefilter.h"
#include "processors/itk_ImageIntensity/addimagefilter.h"
#include "processors/itk_ImageIntensity/andimagefilter.h"
#include "processors/itk_ImageIntensity/asinimagefilter.h"
#include "processors/itk_ImageIntensity/atan2imagefilter.h"
#include "processors/itk_ImageIntensity/atanimagefilter.h"
#include "processors/itk_ImageIntensity/binarymagnitudeimagefilter.h"
#include "processors/itk_ImageIntensity/boundedreciprocalimagefilter.h"
#include "processors/itk_ImageIntensity/constrainedvalueadditionimagefilter.h"
#include "processors/itk_ImageIntensity/constrainedvaluedifferenceimagefilter.h"
#include "processors/itk_ImageIntensity/cosimagefilter.h"
#include "processors/itk_ImageIntensity/divideimagefilter.h"
#include "processors/itk_ImageIntensity/edgepotentialimagefilter.h"
#include "processors/itk_ImageIntensity/expimagefilter.h"
#include "processors/itk_ImageIntensity/expnegativeimagefilter.h"
#include "processors/itk_ImageIntensity/intensitywindowingimagefilter.h"
#include "processors/itk_ImageIntensity/invertintensityimagefilter.h"
#include "processors/itk_ImageIntensity/log10imagefilter.h"
#include "processors/itk_ImageIntensity/logimagefilter.h"
#include "processors/itk_ImageIntensity/maskimagefilter.h"
#include "processors/itk_ImageIntensity/maximumimagefilter.h"
#include "processors/itk_ImageIntensity/minimumimagefilter.h"
#include "processors/itk_ImageIntensity/modulusimagefilter.h"
#include "processors/itk_ImageIntensity/multiplyimagefilter.h"
#include "processors/itk_ImageIntensity/notimagefilter.h"
#include "processors/itk_ImageIntensity/orimagefilter.h"
#include "processors/itk_ImageIntensity/rescaleintensityimagefilter.h"
#include "processors/itk_ImageIntensity/shiftscaleimagefilter.h"
#include "processors/itk_ImageIntensity/sigmoidimagefilter.h"
#include "processors/itk_ImageIntensity/sinimagefilter.h"
#include "processors/itk_ImageIntensity/sqrtimagefilter.h"
#include "processors/itk_ImageIntensity/squareimagefilter.h"
#include "processors/itk_ImageIntensity/subtractimagefilter.h"
#include "processors/itk_ImageIntensity/tanimagefilter.h"
#include "processors/itk_ImageIntensity/ternaryaddimagefilter.h"
#include "processors/itk_ImageIntensity/ternarymagnitudeimagefilter.h"
#include "processors/itk_ImageIntensity/ternarymagnitudesquaredimagefilter.h"
#include "processors/itk_ImageIntensity/weightedaddimagefilter.h"
#include "processors/itk_ImageIntensity/xorimagefilter.h"
#include "processors/itk_ImageLabel/binarycontourimagefilter.h"
#include "processors/itk_ImageLabel/labelcontourimagefilter.h"
#include "processors/itk_ImageStatistics/accumulateimagefilter.h"
#include "processors/itk_ImageStatistics/adaptivehistogramequalizationimagefilter.h"
#include "processors/itk_ImageStatistics/getaveragesliceimagefilter.h"
#include "processors/itk_ImageStatistics/minimummaximumimagefilter.h"
#include "processors/itk_LabelVoting/binarymedianimagefilter.h"
#include "processors/itk_LabelVoting/labelvotingimagefilter.h"
#include "processors/itk_LabelVoting/votingbinaryholefillingimagefilter.h"
#include "processors/itk_LabelVoting/votingbinaryiterativeholefillingimagefilter.h"
#include "processors/itk_LevelSets/curveslevelsetimagefilter.h"
#include "processors/itk_LevelSets/geodesicactivecontourlevelsetimagefilter.h"
#include "processors/itk_LevelSets/geodesicactivecontourshapepriorlevelsetimagefilter.h"
#include "processors/itk_LevelSets/narrowbandcurveslevelsetimagefilter.h"
#include "processors/itk_LevelSets/narrowbandthresholdsegmentationlevelsetimagefilter.h"
#include "processors/itk_LevelSets/shapedetectionlevelsetimagefilter.h"
#include "processors/itk_MathematicalMorphology/basicdilateimagefilter.h"
#include "processors/itk_MathematicalMorphology/basicerodeimagefilter.h"
#include "processors/itk_MathematicalMorphology/blacktophatimagefilter.h"
#include "processors/itk_MathematicalMorphology/closingbyreconstructionimagefilter.h"
#include "processors/itk_MathematicalMorphology/doublethresholdimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscaleconnectedclosingimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscaleconnectedopeningimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscaledilateimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscaleerodeimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscalefillholeimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscalefunctiondilateimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscalefunctionerodeimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscalegeodesicdilateimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscalegeodesicerodeimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscalegrindpeakimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscalemorphologicalclosingimagefilter.h"
#include "processors/itk_MathematicalMorphology/grayscalemorphologicalopeningimagefilter.h"
#include "processors/itk_MathematicalMorphology/hconcaveimagefilter.h"
#include "processors/itk_MathematicalMorphology/hconveximagefilter.h"
#include "processors/itk_MathematicalMorphology/hmaximaimagefilter.h"
#include "processors/itk_MathematicalMorphology/hminimaimagefilter.h"
#include "processors/itk_MathematicalMorphology/morphologicalgradientimagefilter.h"
#include "processors/itk_MathematicalMorphology/movinghistogramdilateimagefilter.h"
#include "processors/itk_MathematicalMorphology/movinghistogramerodeimagefilter.h"
#include "processors/itk_MathematicalMorphology/movinghistogrammorphologicalgradientimagefilter.h"
#include "processors/itk_MathematicalMorphology/openingbyreconstructionimagefilter.h"
#include "processors/itk_MathematicalMorphology/reconstructionbydilationimagefilter.h"
#include "processors/itk_MathematicalMorphology/reconstructionbyerosionimagefilter.h"
#include "processors/itk_MathematicalMorphology/whitetophatimagefilter.h"
#include "processors/itk_RegionGrowing/confidenceconnectedimagefilter.h"
#include "processors/itk_RegionGrowing/connectedthresholdimagefilter.h"
#include "processors/itk_RegionGrowing/isolatedconnectedimagefilter.h"
#include "processors/itk_RegionGrowing/neighborhoodconnectedimagefilter.h"
#include "processors/itk_Smoothing/binomialblurimagefilter.h"
#include "processors/itk_Smoothing/discretegaussianimagefilter.h"
#include "processors/itk_Smoothing/meanimagefilter.h"
#include "processors/itk_Smoothing/medianimagefilter.h"
#include "processors/itk_Smoothing/recursivegaussianimagefilter.h"
#include "processors/itk_Smoothing/smoothingrecursivegaussianimagefilter.h"
#include "processors/itk_Thresholding/binarythresholdimagefilter.h"
#include "processors/itk_Thresholding/otsumultiplethresholdsimagefilter.h"
#include "processors/itk_Thresholding/otsuthresholdimagefilter.h"
#include "processors/itk_Thresholding/thresholdimagefilter.h"
#include "processors/itk_Watersheds/isolatedwatershedimagefilter.h"


namespace voreen {

ITK_GENERATEDModule::ITK_GENERATEDModule(const std::string& modulePath)
    : VoreenModule(modulePath)
{
    setID("ITK_GENERATED");
    setGuiName("ITK_GENERATED");

    registerProcessor(new CurvatureAnisotropicDiffusionImageFilterITK());
    registerProcessor(new GradientAnisotropicDiffusionImageFilterITK());
    registerProcessor(new VectorCurvatureAnisotropicDiffusionImageFilterITK());
    registerProcessor(new VectorGradientAnisotropicDiffusionImageFilterITK());
    registerProcessor(new AntiAliasBinaryImageFilterITK());
    registerProcessor(new BinaryDilateImageFilterITK());
    registerProcessor(new BinaryErodeImageFilterITK());
    registerProcessor(new BinaryMorphologicalClosingImageFilterITK());
    registerProcessor(new BinaryMorphologicalOpeningImageFilterITK());
    registerProcessor(new BinaryPruningImageFilterITK());
    registerProcessor(new DilateObjectMorphologyImageFilterITK());
    registerProcessor(new ErodeObjectMorphologyImageFilterITK());
    registerProcessor(new ConnectedComponentImageFilterITK());
    registerProcessor(new HardConnectedComponentImageFilterITK());
    registerProcessor(new RelabelComponentImageFilterITK());
    registerProcessor(new ScalarConnectedComponentImageFilterITK());
    registerProcessor(new ThresholdMaximumConnectedComponentsImageFilterITK());
    registerProcessor(new CurvatureFlowImageFilterITK());
    registerProcessor(new MinMaxCurvatureFlowImageFilterITK());
    registerProcessor(new ContourDirectedMeanDistanceImageFilterITK());
    registerProcessor(new ContourMeanDistanceImageFilterITK());
    registerProcessor(new DanielssonDistanceMapImageFilterITK());
    registerProcessor(new DirectedHausdorffDistanceImageFilterITK());
    registerProcessor(new HausdorffDistanceImageFilterITK());
    registerProcessor(new SignedDanielssonDistanceMapImageFilterITK());
    registerProcessor(new AbsoluteValueDifferenceImageFilterITK());
    registerProcessor(new CheckerBoardImageFilterITK());
    registerProcessor(new SimilarityIndexImageFilterITK());
    registerProcessor(new SquaredDifferenceImageFilterITK());
    registerProcessor(new CannyEdgeDetectionImageFilterITK());
    registerProcessor(new DerivativeImageFilterITK());
    registerProcessor(new LaplacianImageFilterITK());
    registerProcessor(new LaplacianRecursiveGaussianImageFilterITK());
    registerProcessor(new LaplacianSharpeningImageFilterITK());
    registerProcessor(new SimpleContourExtractorImageFilterITK());
    registerProcessor(new SobelEdgeDetectionImageFilterITK());
    registerProcessor(new ZeroCrossingBasedEdgeDetectionImageFilterITK());
    registerProcessor(new ZeroCrossingImageFilterITK());
    registerProcessor(new LabelOverlayImageFilterITK());
    registerProcessor(new LabelToRGBImageFilterITK());
    registerProcessor(new GradientImageFilterITK());
    registerProcessor(new GradientMagnitudeImageFilterITK());
    registerProcessor(new GradientMagnitudeRecursiveGaussianImageFilterITK());
    registerProcessor(new GradientRecursiveGaussianImageFilterITK());
    registerProcessor(new VectorGradientMagnitudeImageFilterITK());
    registerProcessor(new AbsImageFilterITK());
    registerProcessor(new AcosImageFilterITK());
    registerProcessor(new AddImageFilterITK());
    registerProcessor(new AndImageFilterITK());
    registerProcessor(new AsinImageFilterITK());
    registerProcessor(new Atan2ImageFilterITK());
    registerProcessor(new AtanImageFilterITK());
    registerProcessor(new BinaryMagnitudeImageFilterITK());
    registerProcessor(new BoundedReciprocalImageFilterITK());
    registerProcessor(new ConstrainedValueAdditionImageFilterITK());
    registerProcessor(new ConstrainedValueDifferenceImageFilterITK());
    registerProcessor(new CosImageFilterITK());
    registerProcessor(new DivideImageFilterITK());
    registerProcessor(new EdgePotentialImageFilterITK());
    registerProcessor(new ExpImageFilterITK());
    registerProcessor(new ExpNegativeImageFilterITK());
    registerProcessor(new IntensityWindowingImageFilterITK());
    registerProcessor(new InvertIntensityImageFilterITK());
    registerProcessor(new Log10ImageFilterITK());
    registerProcessor(new LogImageFilterITK());
    registerProcessor(new MaskImageFilterITK());
    registerProcessor(new MaximumImageFilterITK());
    registerProcessor(new MinimumImageFilterITK());
    registerProcessor(new ModulusImageFilterITK());
    registerProcessor(new MultiplyImageFilterITK());
    registerProcessor(new NotImageFilterITK());
    registerProcessor(new OrImageFilterITK());
    registerProcessor(new RescaleIntensityImageFilterITK());
    registerProcessor(new ShiftScaleImageFilterITK());
    registerProcessor(new SigmoidImageFilterITK());
    registerProcessor(new SinImageFilterITK());
    registerProcessor(new SqrtImageFilterITK());
    registerProcessor(new SquareImageFilterITK());
    registerProcessor(new SubtractImageFilterITK());
    registerProcessor(new TanImageFilterITK());
    registerProcessor(new TernaryAddImageFilterITK());
    registerProcessor(new TernaryMagnitudeImageFilterITK());
    registerProcessor(new TernaryMagnitudeSquaredImageFilterITK());
    registerProcessor(new WeightedAddImageFilterITK());
    registerProcessor(new XorImageFilterITK());
    registerProcessor(new BinaryContourImageFilterITK());
    registerProcessor(new LabelContourImageFilterITK());
    registerProcessor(new AccumulateImageFilterITK());
    registerProcessor(new AdaptiveHistogramEqualizationImageFilterITK());
    registerProcessor(new GetAverageSliceImageFilterITK());
    registerProcessor(new MinimumMaximumImageFilterITK());
    registerProcessor(new BinaryMedianImageFilterITK());
    registerProcessor(new LabelVotingImageFilterITK());
    registerProcessor(new VotingBinaryHoleFillingImageFilterITK());
    registerProcessor(new VotingBinaryIterativeHoleFillingImageFilterITK());
    registerProcessor(new CurvesLevelSetImageFilterITK());
    registerProcessor(new GeodesicActiveContourLevelSetImageFilterITK());
    registerProcessor(new GeodesicActiveContourShapePriorLevelSetImageFilterITK());
    registerProcessor(new NarrowBandCurvesLevelSetImageFilterITK());
    registerProcessor(new NarrowBandThresholdSegmentationLevelSetImageFilterITK());
    registerProcessor(new ShapeDetectionLevelSetImageFilterITK());
    registerProcessor(new BasicDilateImageFilterITK());
    registerProcessor(new BasicErodeImageFilterITK());
    registerProcessor(new BlackTopHatImageFilterITK());
    registerProcessor(new ClosingByReconstructionImageFilterITK());
    registerProcessor(new DoubleThresholdImageFilterITK());
    registerProcessor(new GrayscaleConnectedClosingImageFilterITK());
    registerProcessor(new GrayscaleConnectedOpeningImageFilterITK());
    registerProcessor(new GrayscaleDilateImageFilterITK());
    registerProcessor(new GrayscaleErodeImageFilterITK());
    registerProcessor(new GrayscaleFillholeImageFilterITK());
    registerProcessor(new GrayscaleFunctionDilateImageFilterITK());
    registerProcessor(new GrayscaleFunctionErodeImageFilterITK());
    registerProcessor(new GrayscaleGeodesicDilateImageFilterITK());
    registerProcessor(new GrayscaleGeodesicErodeImageFilterITK());
    registerProcessor(new GrayscaleGrindPeakImageFilterITK());
    registerProcessor(new GrayscaleMorphologicalClosingImageFilterITK());
    registerProcessor(new GrayscaleMorphologicalOpeningImageFilterITK());
    registerProcessor(new HConcaveImageFilterITK());
    registerProcessor(new HConvexImageFilterITK());
    registerProcessor(new HMaximaImageFilterITK());
    registerProcessor(new HMinimaImageFilterITK());
    registerProcessor(new MorphologicalGradientImageFilterITK());
    registerProcessor(new MovingHistogramDilateImageFilterITK());
    registerProcessor(new MovingHistogramErodeImageFilterITK());
    registerProcessor(new MovingHistogramMorphologicalGradientImageFilterITK());
    registerProcessor(new OpeningByReconstructionImageFilterITK());
    registerProcessor(new ReconstructionByDilationImageFilterITK());
    registerProcessor(new ReconstructionByErosionImageFilterITK());
    registerProcessor(new WhiteTopHatImageFilterITK());
    registerProcessor(new ConfidenceConnectedImageFilterITK());
    registerProcessor(new ConnectedThresholdImageFilterITK());
    registerProcessor(new IsolatedConnectedImageFilterITK());
    registerProcessor(new NeighborhoodConnectedImageFilterITK());
    registerProcessor(new BinomialBlurImageFilterITK());
    registerProcessor(new DiscreteGaussianImageFilterITK());
    registerProcessor(new MeanImageFilterITK());
    registerProcessor(new MedianImageFilterITK());
    registerProcessor(new RecursiveGaussianImageFilterITK());
    registerProcessor(new SmoothingRecursiveGaussianImageFilterITK());
    registerProcessor(new BinaryThresholdImageFilterITK());
    registerProcessor(new OtsuMultipleThresholdsImageFilterITK());
    registerProcessor(new OtsuThresholdImageFilterITK());
    registerProcessor(new ThresholdImageFilterITK());
    registerProcessor(new IsolatedWatershedImageFilterITK());

}

} // namespace