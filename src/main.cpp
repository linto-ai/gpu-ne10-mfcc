/**
 * Copyright (C) 2018 Linagora
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <thread>
#include <unistd.h>

#include "../include/recordmanager.h"
#include "../include/audio.h"
#include "../include/mfcc.h"
using namespace std;

int main(int argc, char* argv[])
{
    AudioParameter audioParams;
        audioParams.sampleRate = 16000;
        audioParams.channels = 1;
        audioParams.chunkSize = 1024;
    AudioInput* input = new AudioInput(&audioParams);
    Record_Manager *manager = new Record_Manager("test.raw",false,"meeting",4000,1024);
    MFCC *mfcc = new MFCC((size_t)400,0.97,povey);
    // Link modules
    mfcc->setInput(input->subscribe());
    int16_t data[400] = {0,-4,3,4,-30,74,-134,200,-366,-2833,-3100,-2900,-4112,-4454,-3930,-2747,-780,472,1039,433,-1554,-2818,-4009,-5290,-5485,-4979,-4700,-5236,-6363,-7281,-7016,-5939,-5185,-4662,-3536,-2831,-2301,-1596,-1772,-1518,-1235,-1946,-2924,-3689,-3782,-3363,-3677,-4667,-5243,-5604,-5882,-5770,-4868,-3630,-3227,-3369,-3585,-3270,-2372,-1481,-1144,-1007,-1043,-1185,-1578,-2361,-3285,-3561,-3067,-2659,-1670,-304,416,1609,2681,3556,4446,4480,3205,910,-929,-1924,-2912,-4110,-4329,-3789,-3323,-3066,-2405,-1011,609,2001,2794,2763,2511,2134,1776,1304,537,-34,-562,-752,-662,-542,288,795,942,1526,1610,734,-333,-673,-870,-1488,-1627,-1442,-1148,-618,-1168,-637,1150,1927,2785,3223,2831,2356,1258,168,-403,-773,-953,-1114,-1227,-1586,-1637,-1301,-1831,-2126,-2348,-3015,-2879,-2233,-1758,-744,448,614,660,808,67,-637,-200,395,439,767,867,677,650,461,962,1516,1443,1929,2499,2031,880,-23,-290,-806,-1778,-2106,-1684,-1314,-1340,-1181,-808,-33,1428,2301,2464,2566,2996,2952,2332,1788,1399,1009,118,-660,-970,-1423,-1495,-887,393,1620,2111,2865,3099,2576,2430,2143,1856,1269,459,-281,-593,218,949,929,1278,1461,1936,3278,3781,3019,1817,564,110,63,237,1109,1881,2591,2790,2456,2592,2487,2163,2106,1675,712,-246,-213,-269,-505,47,890,2291,3918,3999,2877,1902,1309,282,-691,-817,-916,-1324,-2393,-3445,-3237,-2597,-2004,-1225,-685,-214,333,741,887,925,1088,659,-99,-371,-454,-282,790,1613,1561,1772,1621,700,-359,-759,-1359,-1808,-1853,-1768,-1544,-1392,-1063,-593,484,1374,1965,2852,3061,2961,2761,2123,1835,1374,-57,-1422,-2089,-2272,-1532,-471,371,1520,2947,3517,3489,3208,2464,1558,601,-1033,-2630,-3378,-3722,-3485,-2942,-1680,676,2750,3501,4096,4491,4793,5785,5619,4317,2979,1676,441,-440,-431,-68,99,160,100,380,916,1201,1414,1841,2294,2422,2477,2496,2224,2440,2754,2882,2769,2707,2888,3512,3992,3290,2437,2469,2177,1687,1482,913,889,1538,2272,2279,1743,969,470,-192,-1178,-1301,-1112,-1231,-1143,-701,37,947,1784,2136,2013,1913,1546,729,382,515,362,-282,-2148,-3500,-3170,-3014,-3446,-3426,-3777,-4452,-4293,-4147,-4057,-3913,-3627,-3191,-2767,-1906,-1072,-813};
    mfcc->computeFrame(data);
    mfcc->computeFrame(data);
    return 0;
}