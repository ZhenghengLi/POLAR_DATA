# POLAR_DATA

[POLAR](http://isdc.unige.ch/polar) is a Gamma-Ray Bursts (GRB) polarization detector and was launched as a payload of China's space laboratory Tiangong-2 on 15th September 2016.

This project was started on 19th September 2015 by Zhengheng Li after he came to [IHEP](http://english.ihep.cas.cn). This project is mainly for doing the decoding and pre-processing of the raw data produced by [POLAR](http://polar.ihep.ac.cn/en) detector from both ground and in-orbit test and generating the high level scientific data products for POLAR mission as well as all the latter related science data analysis like the in-orbit calibration and the polarization analysis for the detected GRB data sample. In the project C++ is the main program language while the [ROOT](https://root.cern.ch) and [boost](https://www.boost.org) libraries are widely used. Python is also heavily used mainly for the pipeline scripts.

The paper describing all the details of the in-orbit calibration for POLAR (under folder "Analysis_SCI_1Q") was published on 22nd May 2018:  
Zhengheng Li, et al. In-orbit instrument performance study and calibration for POLAR polarization measurements. Nuclear Inst. and Methods in Physics Research, A 900 (2018) 8–24. doi:[10.1016/j.nima.2018.05.041](https://doi.org/10.1016/j.nima.2018.05.041). arXiv:[1805.07605](https://arxiv.org/abs/1805.07605).  
An archived repository containing the paper and its source code is [In-Orbit_Instrument_Performance](https://github.com/ZhenghengLi/In-Orbit_Instrument_Performance).  
For any work where the code in the project is used as well as the calibration files generated by the code, the user is required to cite this NIM publication.

The paper describing the data pre-processing pipeline of POLAR (under folder "Preprocessing") was published in July of 2019:  
Zheng-Heng Li et al. Observation data pre-processing and scientific data products generation of POLAR. Research in Astronomy and Astrophysics, 19(7):91, 2019. doi:[10.1088/1674-4527/19/7/91](https://doi.org/10.1088/1674-4527/19/7/91). arXiv:[1901.00800](https://arxiv.org/abs/1901.00800).  
An archived repository containing the paper and its source code is [Observation_Data_Pre-Processing](https://github.com/ZhenghengLi/Observation_Data_Pre-Processing).

