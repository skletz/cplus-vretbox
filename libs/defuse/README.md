# DeFUSE
DeFuse is a collection of classes in C++ for extracting and evaluating video-based features for similarity search. The purpose of this project arose due to my master thesis. The idea was to develop an extension of a content descriptor, which is called static feature signatures that has been introduced for image retrieval tasks. Several attempts and different minor projects later resulted finally in the collection of these classes.

The following feature extractors are available:.
- Feature Signatures
- Flow-based Dynamic Feature Signatures

## Feature Xtractors

### Feature Signatures (SIGXtractor)
Feature Signatures are based on an adaptive-binning histograms. Beecks et al. [^fn1] defined the basic concept of feature signatures due to the work of Rubner et al. [^fn2] and Leow et al. [^fn3] in the context of content-based image retrieval as visual content descriptor. 

SIGXtractor is based on PCTSignatures source code, which is an implementation of feature signatures focus on position, color and texture features. It was originally implemented to contribute to the [OpenCV's extra modules](https://github.com/opencv/opencv_contrib). The module was a preliminary version which was received in 2015. In more detail, this module implements PCT (position-color-texture) [^fn4]  signature and SQFD (Signature Quadratic Form Distance) [^fn5] .

[^fn1]: Beecks, C., Uysal, M. S., & Seidl, T. (2009). [Signature Quadratic Form Distances for Content-Based Similarity](http://dl.acm.org/citation.cfm?id=1631391). Proceedings of the Seventeen ACM International Conference on Multimedia, 697. 

[^fn2]: Rubner, Y., Tomasi, C., & Guibas, L. J. (2000). [The Earth Mover�s Distance as a Metric for Image Retrieval](https://link.springer.com/article/10.1023/A:1026543900054). International Journal of Computer Vision, 40(2), 99�121.

[^fn3]: Leow, W. K., & Li, R. (2001). [Adaptive Binning and Dissimilarity Measure for Image Retrieval and Classification](http://ieeexplore.ieee.org/document/990965/). Proceedings of the 2001 IEEE Computer Society Conference on Computer Vision and Pattern Recognition. CVPR 2001, 2, 234�239.

[^fn4]:Kruli�, M., Lokoc, J., & Skopal, T. (2016). [Efficient extraction of clustering-based feature signatures using GPU architectures](https://link.springer.com/article/10.1007/s11042-015-2726-y). Multimedia Tools and Applications, 75(13), 8071�8103. 
[^fn5]:Beecks, C., Uysal, M. S., & Seidl, T. (2010). [Signature Quadratic Form Distance](http://dl.acm.org/citation.cfm?id=1816105). In Proceedings of the ACM International Conference on Image and Video Retrieval (pp. 438�445).

### Flow-based Dynmaic Feature Signatures (FDSIGXtractor)