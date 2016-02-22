# POLAR_RawData_Decode_Project

This project was started at Sep 19, 2015 after I came to IHEP and proposed to do the decoding of the raw data produced from POLAR detector and all the latter related science analysis. C++ is the main language and the ROOT library is used. The raw data will be transformed and stored to .root files. Monte Carlo simulation using Geant4 will also be included into this project because the data analysis is very dependent on the result of simulation.

This project is now under developing ...

## Data Structure Convention

### struct Modules_T

Type               | Name               | Discription
-------------------|--------------------|------------------------
Long64_t           | trigg_num          | Sequential number of the trigger packet of an event. Start from 0, -1 if it has no corresponding trigger packet. -2 when bad. Pedestal and no pedestal packets use different number counter.
Long64_t           | event_num          | Sequential number of the event packet of a module. Start from 0. -1 when bad. Pedestal and no pedestal packets use different number counter.
Long64_t           | event_num_g        | Order number at the sequence of appearing in the raw data file. Start from 0. -1 when bad. Pedestal and no pedestal packets use the same number counter.

### struct Trigger_T

Type               | Name               | Discription
-------------------|--------------------|------------------------
Long64_t           | trigg_num          | Sequential number of the trigger packet. Start from 0. -1 when bad. Pedestal and no pedestal packets use different number counter.
Long64_t           | trigg_num_g        | Order number at the sequence of appearing in the raw data file. Start from 0. Pedestal and no pedestal packets use the same number counter.


