#include <Audio.h>
#include <Wire.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=101,401
AudioMixer4              mixer1;         //xy=256,414
AudioAnalyzePeak         peak1;          //xy=286,140
AudioFilterStateVariable filter1;        //xy=486,47
AudioFilterStateVariable filter2;        //xy=487,97
AudioFilterStateVariable filter3;        //xy=487,149
AudioFilterStateVariable filter4;        //xy=487,199
AudioFilterStateVariable filter5;        //xy=488,251
AudioFilterStateVariable filter6;        //xy=490,302
AudioFilterStateVariable filter7;        //xy=492,352
AudioFilterStateVariable filter9;        //xy=492,456
AudioFilterStateVariable filter8;        //xy=493,402
AudioFilterStateVariable filter10;       //xy=493,506
AudioFilterStateVariable filter11;       //xy=493,558
AudioFilterStateVariable filter12;       //xy=493,608
AudioFilterStateVariable filter13;       //xy=494,660
AudioFilterStateVariable filter14;       //xy=496,711
AudioFilterStateVariable filter15;       //xy=498,761
AudioFilterStateVariable filter16;       //xy=499,811
AudioMixer4              mixer10;        //xy=637,1256
AudioFilterStateVariable Bfilter4;       //xy=647,198
AudioFilterStateVariable Bfilter1;       //xy=649,40
AudioFilterStateVariable Bfilter2;       //xy=649,97
AudioFilterStateVariable Bfilter3;       //xy=649,147
AudioFilterStateVariable Bfilter5;       //xy=649,248
AudioFilterStateVariable Bfilter6;       //xy=651,302
AudioFilterStateVariable Bfilter9;       //xy=651,450
AudioFilterStateVariable Bfilter10;      //xy=651,501
AudioFilterStateVariable Bfilter8;       //xy=652,406
AudioFilterStateVariable Bfilter7;       //xy=653,353
AudioFilterStateVariable Bfilter11;      //xy=654,555
AudioFilterStateVariable Bfilter12;      //xy=654,607
AudioFilterStateVariable Bfilter13;      //xy=655,661
AudioFilterStateVariable Bfilter14;      //xy=656,713
AudioFilterStateVariable Bfilter15;      //xy=656,766
AudioFilterStateVariable Bfilter16;      //xy=658,826
AudioEffectRectifier     rectify2;       //xy=824,103
AudioEffectRectifier     rectify5;       //xy=824,255
AudioEffectRectifier     rectify3;       //xy=825,154
AudioEffectRectifier     rectify4;       //xy=826,203
AudioEffectRectifier     rectify1;       //xy=827,39
AudioEffectRectifier     rectify6;       //xy=827,307
AudioEffectRectifier     rectify9;       //xy=828,461
AudioEffectRectifier     rectify8;       //xy=830,406
AudioEffectRectifier     rectify10;      //xy=830,511
AudioEffectRectifier     rectify7;       //xy=831,357
AudioEffectRectifier     rectify11;      //xy=831,563
AudioEffectRectifier     rectify12;      //xy=832,613
AudioEffectRectifier     rectify14;      //xy=834,716
AudioEffectRectifier     rectify13;      //xy=837,665
AudioEffectRectifier     rectify16;      //xy=839,828
AudioEffectRectifier     rectify15;      //xy=840,765
AudioFilterBiquad        biquad5;        //xy=963,254
AudioFilterBiquad        biquad1;        //xy=964,40
AudioFilterBiquad        biquad7;        //xy=963,357
AudioFilterBiquad        biquad4;        //xy=964,203
AudioFilterBiquad        biquad6;        //xy=964,306
AudioFilterBiquad        biquad2;        //xy=967,102
AudioFilterBiquad        biquad3;        //xy=967,155
AudioFilterBiquad        biquad9;        //xy=968,462
AudioFilterBiquad        biquad8;        //xy=971,406
AudioFilterBiquad        biquad11;       //xy=972,563
AudioFilterBiquad        biquad12;       //xy=973,613
AudioFilterBiquad        biquad10;       //xy=974,512
AudioFilterBiquad        biquad14;       //xy=977,715
AudioFilterBiquad        biquad13;       //xy=979,665
AudioFilterBiquad        biquad15;       //xy=984,765
AudioFilterBiquad        biquad16;       //xy=986,828
AudioSynthNoiseWhite     noise1;         //xy=1155,1725
AudioFilterStateVariable filter1B;       //xy=1337,990
AudioFilterStateVariable filter2B;       //xy=1337,1041
AudioFilterStateVariable filter3B;       //xy=1338,1092
AudioFilterStateVariable filter4B;       //xy=1338,1142
AudioFilterStateVariable filter5B;       //xy=1339,1194
AudioFilterStateVariable filter6B;       //xy=1341,1245
AudioFilterStateVariable filter7B;       //xy=1343,1295
AudioFilterStateVariable filter9B;       //xy=1343,1399
AudioFilterStateVariable filter8B;       //xy=1344,1345
AudioFilterStateVariable filter10B;      //xy=1344,1449
AudioFilterStateVariable filter11B;      //xy=1344,1501
AudioFilterStateVariable filter12B;      //xy=1344,1551
AudioFilterStateVariable filter13B;      //xy=1345,1603
AudioFilterStateVariable filter14B;      //xy=1347,1654
AudioFilterStateVariable filter15B;      //xy=1349,1704
AudioFilterStateVariable filter16B;      //xy=1350,1754
AudioFilterStateVariable Cfilter1;       //xy=1520,970
AudioFilterStateVariable Cfilter2;       //xy=1524,1027
AudioFilterStateVariable Cfilter3;       //xy=1525,1085
AudioFilterStateVariable Cfilter4;       //xy=1526,1141
AudioFilterStateVariable Cfilter5;       //xy=1527,1194
AudioFilterStateVariable Cfilter7;       //xy=1528,1299
AudioFilterStateVariable Cfilter6;       //xy=1530,1247
AudioFilterStateVariable Cfilter10;      //xy=1531,1450
AudioFilterStateVariable Cfilter8;       //xy=1532,1350
AudioFilterStateVariable Cfilter16;      //xy=1532,1763
AudioFilterStateVariable Cfilter9;       //xy=1534,1400
AudioFilterStateVariable Cfilter11;      //xy=1537,1501
AudioFilterStateVariable Cfilter14;      //xy=1539,1654
AudioFilterStateVariable Cfilter15;      //xy=1539,1705
AudioFilterStateVariable Cfilter12;      //xy=1541,1554
AudioFilterStateVariable Cfilter13;      //xy=1544,1605
AudioEffectMultiply      multiply6;      //xy=1785,1237
AudioEffectMultiply      multiply2;      //xy=1786,1028
AudioEffectMultiply      multiply7;      //xy=1786,1286
AudioEffectMultiply      multiply9;      //xy=1786,1390
AudioEffectMultiply      multiply15;     //xy=1786,1694
AudioEffectMultiply      multiply1;      //xy=1789,969
AudioEffectMultiply      multiply16;     //xy=1786,1743
AudioEffectMultiply      multiply8;      //xy=1788,1339
AudioEffectMultiply      multiply4;      //xy=1789,1134
AudioEffectMultiply      multiply10;     //xy=1788,1441
AudioEffectMultiply      multiply3;      //xy=1790,1080
AudioEffectMultiply      multiply5;      //xy=1790,1185
AudioEffectMultiply      multiply13;     //xy=1789,1592
AudioEffectMultiply      multiply14;     //xy=1789,1642
AudioEffectMultiply      multiply11;     //xy=1790,1491
AudioEffectMultiply      multiply12;     //xy=1795,1543
AudioMixer4              mixer4;         //xy=1993,1460
AudioMixer4              mixer2;         //xy=1995,1053
AudioMixer4              mixer5;         //xy=1993,1650
AudioMixer4              mixer3;         //xy=1998,1236
AudioMixer4              mixer6;         //xy=2297,1366
AudioEffectDelay         delay1;         //xy=2392,1645
AudioMixer4              mixer7;         //xy=2402,1500
AudioMixer4              mixer9;         //xy=2631,1409
AudioOutputI2S           i2s1;           //xy=2805,1577
AudioConnection          patchCord1(i2s2, 0, mixer1, 0);
AudioConnection          patchCord2(i2s2, 0, mixer9, 0);
AudioConnection          patchCord3(i2s2, 1, mixer10, 0);
AudioConnection          patchCord4(i2s2, 1, mixer9, 1);
AudioConnection          patchCord5(mixer1, 0, filter1, 0);
AudioConnection          patchCord6(mixer1, 0, filter2, 0);
AudioConnection          patchCord7(mixer1, 0, filter3, 0);
AudioConnection          patchCord8(mixer1, 0, filter4, 0);
AudioConnection          patchCord9(mixer1, 0, filter5, 0);
AudioConnection          patchCord10(mixer1, 0, filter6, 0);
AudioConnection          patchCord11(mixer1, 0, filter7, 0);
AudioConnection          patchCord12(mixer1, 0, filter8, 0);
AudioConnection          patchCord13(mixer1, 0, filter9, 0);
AudioConnection          patchCord14(mixer1, 0, filter10, 0);
AudioConnection          patchCord15(mixer1, 0, filter11, 0);
AudioConnection          patchCord16(mixer1, 0, filter12, 0);
AudioConnection          patchCord17(mixer1, 0, filter13, 0);
AudioConnection          patchCord18(mixer1, 0, filter14, 0);
AudioConnection          patchCord19(mixer1, 0, filter15, 0);
AudioConnection          patchCord20(mixer1, 0, filter16, 0);
AudioConnection          patchCord21(mixer1, 0, mixer7, 1);
AudioConnection          patchCord22(mixer1, peak1);
AudioConnection          patchCord23(filter1, 0, Bfilter1, 0);
AudioConnection          patchCord24(filter2, 1, Bfilter2, 0);
AudioConnection          patchCord25(filter3, 1, Bfilter3, 0);
AudioConnection          patchCord26(filter4, 1, Bfilter4, 0);
AudioConnection          patchCord27(filter5, 1, Bfilter5, 0);
AudioConnection          patchCord28(filter6, 1, Bfilter6, 0);
AudioConnection          patchCord29(filter7, 1, Bfilter7, 0);
AudioConnection          patchCord30(filter9, 1, Bfilter9, 0);
AudioConnection          patchCord31(filter8, 1, Bfilter8, 0);
AudioConnection          patchCord32(filter10, 1, Bfilter10, 0);
AudioConnection          patchCord33(filter11, 1, Bfilter11, 0);
AudioConnection          patchCord34(filter12, 1, Bfilter12, 0);
AudioConnection          patchCord35(filter13, 1, Bfilter13, 0);
AudioConnection          patchCord36(filter14, 1, Bfilter14, 0);
AudioConnection          patchCord37(filter15, 1, Bfilter15, 0);
AudioConnection          patchCord38(filter16, 2, Bfilter16, 0);
AudioConnection          patchCord39(mixer10, 0, filter1B, 1);
AudioConnection          patchCord40(mixer10, 0, filter2B, 1);
AudioConnection          patchCord41(mixer10, 0, filter3B, 1);
AudioConnection          patchCord42(mixer10, 0, filter4B, 1);
AudioConnection          patchCord43(mixer10, 0, filter5B, 1);
AudioConnection          patchCord44(mixer10, 0, filter6B, 1);
AudioConnection          patchCord45(mixer10, 0, filter7B, 1);
AudioConnection          patchCord46(mixer10, 0, filter8B, 1);
AudioConnection          patchCord47(mixer10, 0, filter9B, 1);
AudioConnection          patchCord48(mixer10, 0, filter10B, 1);
AudioConnection          patchCord49(mixer10, 0, filter11B, 1);
AudioConnection          patchCord50(mixer10, 0, filter12B, 1);
AudioConnection          patchCord51(mixer10, 0, filter13B, 1);
AudioConnection          patchCord52(mixer10, 0, filter14B, 1);
AudioConnection          patchCord53(mixer10, 0, filter15B, 1);
AudioConnection          patchCord54(Bfilter4, 1, rectify4, 0);
AudioConnection          patchCord55(Bfilter1, 0, rectify1, 0);
AudioConnection          patchCord56(Bfilter2, 1, rectify2, 0);
AudioConnection          patchCord57(Bfilter3, 1, rectify3, 0);
AudioConnection          patchCord58(Bfilter5, 1, rectify5, 0);
AudioConnection          patchCord59(Bfilter6, 1, rectify6, 0);
AudioConnection          patchCord60(Bfilter9, 1, rectify9, 0);
AudioConnection          patchCord61(Bfilter10, 1, rectify10, 0);
AudioConnection          patchCord62(Bfilter8, 1, rectify8, 0);
AudioConnection          patchCord63(Bfilter7, 1, rectify7, 0);
AudioConnection          patchCord64(Bfilter11, 1, rectify11, 0);
AudioConnection          patchCord65(Bfilter12, 1, rectify12, 0);
AudioConnection          patchCord66(Bfilter13, 1, rectify13, 0);
AudioConnection          patchCord67(Bfilter14, 1, rectify14, 0);
AudioConnection          patchCord68(Bfilter15, 1, rectify15, 0);
AudioConnection          patchCord69(Bfilter16, 2, rectify16, 0);
AudioConnection          patchCord70(rectify2, biquad2);
AudioConnection          patchCord71(rectify5, biquad5);
AudioConnection          patchCord72(rectify3, biquad3);
AudioConnection          patchCord73(rectify4, biquad4);
AudioConnection          patchCord74(rectify1, biquad1);
AudioConnection          patchCord75(rectify6, biquad6);
AudioConnection          patchCord76(rectify9, biquad9);
AudioConnection          patchCord77(rectify8, biquad8);
AudioConnection          patchCord78(rectify10, biquad10);
AudioConnection          patchCord79(rectify7, biquad7);
AudioConnection          patchCord80(rectify11, biquad11);
AudioConnection          patchCord81(rectify12, biquad12);
AudioConnection          patchCord82(rectify14, biquad14);
AudioConnection          patchCord83(rectify13, biquad13);
AudioConnection          patchCord84(rectify16, biquad16);
AudioConnection          patchCord85(rectify15, biquad15);
AudioConnection          patchCord86(biquad5, 0, multiply5, 0);
AudioConnection          patchCord87(biquad1, 0, multiply1, 0);
AudioConnection          patchCord88(biquad7, 0, multiply7, 0);
AudioConnection          patchCord89(biquad4, 0, multiply4, 0);
AudioConnection          patchCord90(biquad6, 0, multiply6, 0);
AudioConnection          patchCord91(biquad2, 0, multiply2, 0);
AudioConnection          patchCord92(biquad3, 0, multiply3, 0);
AudioConnection          patchCord93(biquad9, 0, multiply9, 0);
AudioConnection          patchCord94(biquad8, 0, multiply8, 0);
AudioConnection          patchCord95(biquad11, 0, multiply11, 0);
AudioConnection          patchCord96(biquad12, 0, multiply12, 0);
AudioConnection          patchCord97(biquad10, 0, multiply10, 0);
AudioConnection          patchCord98(biquad14, 0, multiply14, 0);
AudioConnection          patchCord99(biquad13, 0, multiply13, 0);
AudioConnection          patchCord100(biquad15, 0, multiply15, 0);
AudioConnection          patchCord101(biquad16, 0, multiply16, 0);
AudioConnection          patchCord102(noise1, 0, filter16B, 0);
AudioConnection          patchCord103(filter1B, 0, Cfilter1, 0);
AudioConnection          patchCord104(filter2B, 1, Cfilter2, 0);
AudioConnection          patchCord105(filter3B, 1, Cfilter3, 0);
AudioConnection          patchCord106(filter4B, 1, Cfilter4, 0);
AudioConnection          patchCord107(filter5B, 1, Cfilter5, 0);
AudioConnection          patchCord108(filter6B, 1, Cfilter6, 0);
AudioConnection          patchCord109(filter7B, 1, Cfilter7, 0);
AudioConnection          patchCord110(filter9B, 1, Cfilter9, 0);
AudioConnection          patchCord111(filter8B, 1, Cfilter8, 0);
AudioConnection          patchCord112(filter10B, 1, Cfilter10, 0);
AudioConnection          patchCord113(filter11B, 1, Cfilter11, 0);
AudioConnection          patchCord114(filter12B, 1, Cfilter12, 0);
AudioConnection          patchCord115(filter13B, 1, Cfilter13, 0);
AudioConnection          patchCord116(filter14B, 1, Cfilter14, 0);
AudioConnection          patchCord117(filter15B, 1, Cfilter15, 0);
AudioConnection          patchCord118(filter16B, 2, Cfilter16, 0);
AudioConnection          patchCord119(Cfilter1, 0, multiply1, 1);
AudioConnection          patchCord120(Cfilter2, 1, multiply2, 1);
AudioConnection          patchCord121(Cfilter3, 1, multiply3, 1);
AudioConnection          patchCord122(Cfilter4, 1, multiply4, 1);
AudioConnection          patchCord123(Cfilter5, 1, multiply5, 1);
AudioConnection          patchCord124(Cfilter7, 1, multiply7, 1);
AudioConnection          patchCord125(Cfilter6, 1, multiply6, 1);
AudioConnection          patchCord126(Cfilter10, 1, multiply10, 1);
AudioConnection          patchCord127(Cfilter8, 1, multiply8, 1);
AudioConnection          patchCord128(Cfilter16, 2, multiply16, 1);
AudioConnection          patchCord129(Cfilter9, 1, multiply9, 1);
AudioConnection          patchCord130(Cfilter11, 1, multiply11, 1);
AudioConnection          patchCord131(Cfilter14, 1, multiply14, 1);
AudioConnection          patchCord132(Cfilter15, 1, multiply15, 1);
AudioConnection          patchCord133(Cfilter12, 1, multiply12, 1);
AudioConnection          patchCord134(Cfilter13, 1, multiply13, 1);
AudioConnection          patchCord135(multiply6, 0, mixer3, 1);
AudioConnection          patchCord136(multiply2, 0, mixer2, 1);
AudioConnection          patchCord137(multiply7, 0, mixer3, 2);
AudioConnection          patchCord138(multiply9, 0, mixer4, 0);
AudioConnection          patchCord139(multiply15, 0, mixer5, 2);
AudioConnection          patchCord140(multiply1, 0, mixer2, 0);
AudioConnection          patchCord141(multiply16, 0, mixer5, 3);
AudioConnection          patchCord142(multiply8, 0, mixer3, 3);
AudioConnection          patchCord143(multiply4, 0, mixer2, 3);
AudioConnection          patchCord144(multiply10, 0, mixer4, 1);
AudioConnection          patchCord145(multiply3, 0, mixer2, 2);
AudioConnection          patchCord146(multiply5, 0, mixer3, 0);
AudioConnection          patchCord147(multiply13, 0, mixer5, 0);
AudioConnection          patchCord148(multiply14, 0, mixer5, 1);
AudioConnection          patchCord149(multiply11, 0, mixer4, 2);
AudioConnection          patchCord150(multiply12, 0, mixer4, 3);
AudioConnection          patchCord151(mixer4, 0, mixer6, 2);
AudioConnection          patchCord152(mixer2, 0, mixer6, 0);
AudioConnection          patchCord153(mixer5, 0, mixer6, 3);
AudioConnection          patchCord154(mixer3, 0, mixer6, 1);
AudioConnection          patchCord155(mixer6, 0, mixer7, 0);
AudioConnection          patchCord156(delay1, 0, mixer7, 2);
AudioConnection          patchCord157(mixer7, delay1);
AudioConnection          patchCord158(mixer7, 0, i2s1, 0);
AudioConnection          patchCord159(mixer9, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=144,259
// GUItool: end automatically generated code

