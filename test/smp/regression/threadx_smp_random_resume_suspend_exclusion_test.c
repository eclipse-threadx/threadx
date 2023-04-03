/* Define the ThreadX SMP random resume/suspend/exclude test.  */

#include   <stdio.h>
#include   "tx_api.h"

//#define MAX_PASSES      50000000
//#define MAX_PASSES      50000
#define MAX_PASSES      500


/* Define the ThreadX object control blocks... Must have 256 priority levels... and assumes 4 cores.  */

static TX_THREAD               thread_0;
static TX_THREAD               thread_1;
static TX_THREAD               thread_2;
static TX_THREAD               thread_3;
static TX_THREAD               thread_4;
static TX_THREAD               thread_5;
static TX_THREAD               thread_6;
static TX_THREAD               thread_7;
static TX_THREAD               thread_8;
static TX_THREAD               thread_9;
static TX_THREAD               thread_10;
static TX_THREAD               thread_11;
static TX_THREAD               thread_12;
static TX_THREAD               thread_13;
static TX_THREAD               thread_14;
static TX_THREAD               thread_15;
static TX_THREAD               thread_16;
static TX_THREAD               thread_17;
static TX_THREAD               thread_18;
static TX_THREAD               thread_19;
static TX_THREAD               thread_20;
static TX_THREAD               thread_21;
static TX_THREAD               thread_22;
static TX_THREAD               thread_23;
static TX_THREAD               thread_24;
static TX_THREAD               thread_25;
static TX_THREAD               thread_26;
static TX_THREAD               thread_27;
static TX_THREAD               thread_28;
static TX_THREAD               thread_29;
static TX_THREAD               thread_30;
static TX_THREAD               thread_31;
static TX_THREAD               thread_32;
static TX_THREAD               thread_33;
static TX_THREAD               thread_34;
static TX_THREAD               thread_35;
static TX_THREAD               thread_36;
static TX_THREAD               thread_37;
static TX_THREAD               thread_38;
static TX_THREAD               thread_39;
static TX_THREAD               thread_40;
static TX_THREAD               thread_41;
static TX_THREAD               thread_42;
static TX_THREAD               thread_43;
static TX_THREAD               thread_44;
static TX_THREAD               thread_45;
static TX_THREAD               thread_46;
static TX_THREAD               thread_47;
static TX_THREAD               thread_48;
static TX_THREAD               thread_49;
static TX_THREAD               thread_50;
static TX_THREAD               thread_51;
static TX_THREAD               thread_52;
static TX_THREAD               thread_53;
static TX_THREAD               thread_54;
static TX_THREAD               thread_55;
static TX_THREAD               thread_56;
static TX_THREAD               thread_57;
static TX_THREAD               thread_58;
static TX_THREAD               thread_59;
static TX_THREAD               thread_60;
static TX_THREAD               thread_61;
static TX_THREAD               thread_62;
static TX_THREAD               thread_63;
static TX_THREAD               thread_64;
static TX_THREAD               thread_65;
static TX_THREAD               thread_66;
static TX_THREAD               thread_67;
static TX_THREAD               thread_68;
static TX_THREAD               thread_69;
static TX_THREAD               thread_70;
static TX_THREAD               thread_71;
static TX_THREAD               thread_72;
static TX_THREAD               thread_73;
static TX_THREAD               thread_74;
static TX_THREAD               thread_75;
static TX_THREAD               thread_76;
static TX_THREAD               thread_77;
static TX_THREAD               thread_78;
static TX_THREAD               thread_79;
static TX_THREAD               thread_80;
static TX_THREAD               thread_81;
static TX_THREAD               thread_82;
static TX_THREAD               thread_83;
static TX_THREAD               thread_84;
static TX_THREAD               thread_85;
static TX_THREAD               thread_86;
static TX_THREAD               thread_87;
static TX_THREAD               thread_88;
static TX_THREAD               thread_89;
static TX_THREAD               thread_90;
static TX_THREAD               thread_91;
static TX_THREAD               thread_92;
static TX_THREAD               thread_93;
static TX_THREAD               thread_94;
static TX_THREAD               thread_95;
static TX_THREAD               thread_96;
static TX_THREAD               thread_97;
static TX_THREAD               thread_98;
static TX_THREAD               thread_99;
static TX_THREAD               thread_100;
static TX_THREAD               thread_101;
static TX_THREAD               thread_102;
static TX_THREAD               thread_103;
static TX_THREAD               thread_104;
static TX_THREAD               thread_105;
static TX_THREAD               thread_106;
static TX_THREAD               thread_107;
static TX_THREAD               thread_108;
static TX_THREAD               thread_109;
static TX_THREAD               thread_110;
static TX_THREAD               thread_111;
static TX_THREAD               thread_112;
static TX_THREAD               thread_113;
static TX_THREAD               thread_114;
static TX_THREAD               thread_115;
static TX_THREAD               thread_116;
static TX_THREAD               thread_117;
static TX_THREAD               thread_118;
static TX_THREAD               thread_119;
static TX_THREAD               thread_120;
static TX_THREAD               thread_121;
static TX_THREAD               thread_122;
static TX_THREAD               thread_123;
static TX_THREAD               thread_124;
static TX_THREAD               thread_125;
static TX_THREAD               thread_126;
static TX_THREAD               thread_127;
static TX_THREAD               thread_128;
static TX_THREAD               thread_129;
static TX_THREAD               thread_130;
static TX_THREAD               thread_131;
static TX_THREAD               thread_132;
static TX_THREAD               thread_133;
static TX_THREAD               thread_134;
static TX_THREAD               thread_135;
static TX_THREAD               thread_136;
static TX_THREAD               thread_137;
static TX_THREAD               thread_138;
static TX_THREAD               thread_139;
static TX_THREAD               thread_140;
static TX_THREAD               thread_141;
static TX_THREAD               thread_142;
static TX_THREAD               thread_143;
static TX_THREAD               thread_144;
static TX_THREAD               thread_145;
static TX_THREAD               thread_146;
static TX_THREAD               thread_147;
static TX_THREAD               thread_148;
static TX_THREAD               thread_149;
static TX_THREAD               thread_150;
static TX_THREAD               thread_151;
static TX_THREAD               thread_152;
static TX_THREAD               thread_153;
static TX_THREAD               thread_154;
static TX_THREAD               thread_155;
static TX_THREAD               thread_156;
static TX_THREAD               thread_157;
static TX_THREAD               thread_158;
static TX_THREAD               thread_159;
static TX_THREAD               thread_160;
static TX_THREAD               thread_161;
static TX_THREAD               thread_162;
static TX_THREAD               thread_163;
static TX_THREAD               thread_164;
static TX_THREAD               thread_165;
static TX_THREAD               thread_166;
static TX_THREAD               thread_167;
static TX_THREAD               thread_168;
static TX_THREAD               thread_169;
static TX_THREAD               thread_170;
static TX_THREAD               thread_171;
static TX_THREAD               thread_172;
static TX_THREAD               thread_173;
static TX_THREAD               thread_174;
static TX_THREAD               thread_175;
static TX_THREAD               thread_176;
static TX_THREAD               thread_177;
static TX_THREAD               thread_178;
static TX_THREAD               thread_179;
static TX_THREAD               thread_180;
static TX_THREAD               thread_181;
static TX_THREAD               thread_182;
static TX_THREAD               thread_183;
static TX_THREAD               thread_184;
static TX_THREAD               thread_185;
static TX_THREAD               thread_186;
static TX_THREAD               thread_187;
static TX_THREAD               thread_188;
static TX_THREAD               thread_189;
static TX_THREAD               thread_190;
static TX_THREAD               thread_191;
static TX_THREAD               thread_192;
static TX_THREAD               thread_193;
static TX_THREAD               thread_194;
static TX_THREAD               thread_195;
static TX_THREAD               thread_196;
static TX_THREAD               thread_197;
static TX_THREAD               thread_198;
static TX_THREAD               thread_199;
static TX_THREAD               thread_200;
static TX_THREAD               thread_201;
static TX_THREAD               thread_202;
static TX_THREAD               thread_203;
static TX_THREAD               thread_204;
static TX_THREAD               thread_205;
static TX_THREAD               thread_206;
static TX_THREAD               thread_207;
static TX_THREAD               thread_208;
static TX_THREAD               thread_209;
static TX_THREAD               thread_210;
static TX_THREAD               thread_211;
static TX_THREAD               thread_212;
static TX_THREAD               thread_213;
static TX_THREAD               thread_214;
static TX_THREAD               thread_215;
static TX_THREAD               thread_216;
static TX_THREAD               thread_217;
static TX_THREAD               thread_218;
static TX_THREAD               thread_219;
static TX_THREAD               thread_220;
static TX_THREAD               thread_221;
static TX_THREAD               thread_222;
static TX_THREAD               thread_223;
static TX_THREAD               thread_224;
static TX_THREAD               thread_225;
static TX_THREAD               thread_226;
static TX_THREAD               thread_227;
static TX_THREAD               thread_228;
static TX_THREAD               thread_229;
static TX_THREAD               thread_230;
static TX_THREAD               thread_231;
static TX_THREAD               thread_232;
static TX_THREAD               thread_233;
static TX_THREAD               thread_234;
static TX_THREAD               thread_235;
static TX_THREAD               thread_236;
static TX_THREAD               thread_237;
static TX_THREAD               thread_238;
static TX_THREAD               thread_239;
static TX_THREAD               thread_240;
static TX_THREAD               thread_241;
static TX_THREAD               thread_242;
static TX_THREAD               thread_243;
static TX_THREAD               thread_244;
static TX_THREAD               thread_245;
static TX_THREAD               thread_246;
static TX_THREAD               thread_247;
static TX_THREAD               thread_248;
static TX_THREAD               thread_249;
static TX_THREAD               thread_250;
static TX_THREAD               thread_251;
static TX_THREAD               thread_252;
static TX_THREAD               thread_253;
static TX_THREAD               thread_254;
static TX_THREAD               thread_255;

static TX_THREAD               thread_0_1;
static TX_THREAD               thread_1_1;
static TX_THREAD               thread_2_1;
static TX_THREAD               thread_3_1;
static TX_THREAD               thread_4_1;
static TX_THREAD               thread_5_1;
static TX_THREAD               thread_6_1;
static TX_THREAD               thread_7_1;
static TX_THREAD               thread_8_1;
static TX_THREAD               thread_9_1;
static TX_THREAD               thread_10_1;
static TX_THREAD               thread_11_1;
static TX_THREAD               thread_12_1;
static TX_THREAD               thread_13_1;
static TX_THREAD               thread_14_1;
static TX_THREAD               thread_15_1;
static TX_THREAD               thread_16_1;
static TX_THREAD               thread_17_1;
static TX_THREAD               thread_18_1;
static TX_THREAD               thread_19_1;
static TX_THREAD               thread_20_1;
static TX_THREAD               thread_21_1;
static TX_THREAD               thread_22_1;
static TX_THREAD               thread_23_1;
static TX_THREAD               thread_24_1;
static TX_THREAD               thread_25_1;
static TX_THREAD               thread_26_1;
static TX_THREAD               thread_27_1;
static TX_THREAD               thread_28_1;
static TX_THREAD               thread_29_1;
static TX_THREAD               thread_30_1;
static TX_THREAD               thread_31_1;
static TX_THREAD               thread_32_1;
static TX_THREAD               thread_33_1;
static TX_THREAD               thread_34_1;
static TX_THREAD               thread_35_1;
static TX_THREAD               thread_36_1;
static TX_THREAD               thread_37_1;
static TX_THREAD               thread_38_1;
static TX_THREAD               thread_39_1;
static TX_THREAD               thread_40_1;
static TX_THREAD               thread_41_1;
static TX_THREAD               thread_42_1;
static TX_THREAD               thread_43_1;
static TX_THREAD               thread_44_1;
static TX_THREAD               thread_45_1;
static TX_THREAD               thread_46_1;
static TX_THREAD               thread_47_1;
static TX_THREAD               thread_48_1;
static TX_THREAD               thread_49_1;
static TX_THREAD               thread_50_1;
static TX_THREAD               thread_51_1;
static TX_THREAD               thread_52_1;
static TX_THREAD               thread_53_1;
static TX_THREAD               thread_54_1;
static TX_THREAD               thread_55_1;
static TX_THREAD               thread_56_1;
static TX_THREAD               thread_57_1;
static TX_THREAD               thread_58_1;
static TX_THREAD               thread_59_1;
static TX_THREAD               thread_60_1;
static TX_THREAD               thread_61_1;
static TX_THREAD               thread_62_1;
static TX_THREAD               thread_63_1;
static TX_THREAD               thread_64_1;
static TX_THREAD               thread_65_1;
static TX_THREAD               thread_66_1;
static TX_THREAD               thread_67_1;
static TX_THREAD               thread_68_1;
static TX_THREAD               thread_69_1;
static TX_THREAD               thread_70_1;
static TX_THREAD               thread_71_1;
static TX_THREAD               thread_72_1;
static TX_THREAD               thread_73_1;
static TX_THREAD               thread_74_1;
static TX_THREAD               thread_75_1;
static TX_THREAD               thread_76_1;
static TX_THREAD               thread_77_1;
static TX_THREAD               thread_78_1;
static TX_THREAD               thread_79_1;
static TX_THREAD               thread_80_1;
static TX_THREAD               thread_81_1;
static TX_THREAD               thread_82_1;
static TX_THREAD               thread_83_1;
static TX_THREAD               thread_84_1;
static TX_THREAD               thread_85_1;
static TX_THREAD               thread_86_1;
static TX_THREAD               thread_87_1;
static TX_THREAD               thread_88_1;
static TX_THREAD               thread_89_1;
static TX_THREAD               thread_90_1;
static TX_THREAD               thread_91_1;
static TX_THREAD               thread_92_1;
static TX_THREAD               thread_93_1;
static TX_THREAD               thread_94_1;
static TX_THREAD               thread_95_1;
static TX_THREAD               thread_96_1;
static TX_THREAD               thread_97_1;
static TX_THREAD               thread_98_1;
static TX_THREAD               thread_99_1;
static TX_THREAD               thread_100_1;
static TX_THREAD               thread_101_1;
static TX_THREAD               thread_102_1;
static TX_THREAD               thread_103_1;
static TX_THREAD               thread_104_1;
static TX_THREAD               thread_105_1;
static TX_THREAD               thread_106_1;
static TX_THREAD               thread_107_1;
static TX_THREAD               thread_108_1;
static TX_THREAD               thread_109_1;
static TX_THREAD               thread_110_1;
static TX_THREAD               thread_111_1;
static TX_THREAD               thread_112_1;
static TX_THREAD               thread_113_1;
static TX_THREAD               thread_114_1;
static TX_THREAD               thread_115_1;
static TX_THREAD               thread_116_1;
static TX_THREAD               thread_117_1;
static TX_THREAD               thread_118_1;
static TX_THREAD               thread_119_1;
static TX_THREAD               thread_120_1;
static TX_THREAD               thread_121_1;
static TX_THREAD               thread_122_1;
static TX_THREAD               thread_123_1;
static TX_THREAD               thread_124_1;
static TX_THREAD               thread_125_1;
static TX_THREAD               thread_126_1;
static TX_THREAD               thread_127_1;
static TX_THREAD               thread_128_1;
static TX_THREAD               thread_129_1;
static TX_THREAD               thread_130_1;
static TX_THREAD               thread_131_1;
static TX_THREAD               thread_132_1;
static TX_THREAD               thread_133_1;
static TX_THREAD               thread_134_1;
static TX_THREAD               thread_135_1;
static TX_THREAD               thread_136_1;
static TX_THREAD               thread_137_1;
static TX_THREAD               thread_138_1;
static TX_THREAD               thread_139_1;
static TX_THREAD               thread_140_1;
static TX_THREAD               thread_141_1;
static TX_THREAD               thread_142_1;
static TX_THREAD               thread_143_1;
static TX_THREAD               thread_144_1;
static TX_THREAD               thread_145_1;
static TX_THREAD               thread_146_1;
static TX_THREAD               thread_147_1;
static TX_THREAD               thread_148_1;
static TX_THREAD               thread_149_1;
static TX_THREAD               thread_150_1;
static TX_THREAD               thread_151_1;
static TX_THREAD               thread_152_1;
static TX_THREAD               thread_153_1;
static TX_THREAD               thread_154_1;
static TX_THREAD               thread_155_1;
static TX_THREAD               thread_156_1;
static TX_THREAD               thread_157_1;
static TX_THREAD               thread_158_1;
static TX_THREAD               thread_159_1;
static TX_THREAD               thread_160_1;
static TX_THREAD               thread_161_1;
static TX_THREAD               thread_162_1;
static TX_THREAD               thread_163_1;
static TX_THREAD               thread_164_1;
static TX_THREAD               thread_165_1;
static TX_THREAD               thread_166_1;
static TX_THREAD               thread_167_1;
static TX_THREAD               thread_168_1;
static TX_THREAD               thread_169_1;
static TX_THREAD               thread_170_1;
static TX_THREAD               thread_171_1;
static TX_THREAD               thread_172_1;
static TX_THREAD               thread_173_1;
static TX_THREAD               thread_174_1;
static TX_THREAD               thread_175_1;
static TX_THREAD               thread_176_1;
static TX_THREAD               thread_177_1;
static TX_THREAD               thread_178_1;
static TX_THREAD               thread_179_1;
static TX_THREAD               thread_180_1;
static TX_THREAD               thread_181_1;
static TX_THREAD               thread_182_1;
static TX_THREAD               thread_183_1;
static TX_THREAD               thread_184_1;
static TX_THREAD               thread_185_1;
static TX_THREAD               thread_186_1;
static TX_THREAD               thread_187_1;
static TX_THREAD               thread_188_1;
static TX_THREAD               thread_189_1;
static TX_THREAD               thread_190_1;
static TX_THREAD               thread_191_1;
static TX_THREAD               thread_192_1;
static TX_THREAD               thread_193_1;
static TX_THREAD               thread_194_1;
static TX_THREAD               thread_195_1;
static TX_THREAD               thread_196_1;
static TX_THREAD               thread_197_1;
static TX_THREAD               thread_198_1;
static TX_THREAD               thread_199_1;
static TX_THREAD               thread_200_1;
static TX_THREAD               thread_201_1;
static TX_THREAD               thread_202_1;
static TX_THREAD               thread_203_1;
static TX_THREAD               thread_204_1;
static TX_THREAD               thread_205_1;
static TX_THREAD               thread_206_1;
static TX_THREAD               thread_207_1;
static TX_THREAD               thread_208_1;
static TX_THREAD               thread_209_1;
static TX_THREAD               thread_210_1;
static TX_THREAD               thread_211_1;
static TX_THREAD               thread_212_1;
static TX_THREAD               thread_213_1;
static TX_THREAD               thread_214_1;
static TX_THREAD               thread_215_1;
static TX_THREAD               thread_216_1;
static TX_THREAD               thread_217_1;
static TX_THREAD               thread_218_1;
static TX_THREAD               thread_219_1;
static TX_THREAD               thread_220_1;
static TX_THREAD               thread_221_1;
static TX_THREAD               thread_222_1;
static TX_THREAD               thread_223_1;
static TX_THREAD               thread_224_1;
static TX_THREAD               thread_225_1;
static TX_THREAD               thread_226_1;
static TX_THREAD               thread_227_1;
static TX_THREAD               thread_228_1;
static TX_THREAD               thread_229_1;
static TX_THREAD               thread_230_1;
static TX_THREAD               thread_231_1;
static TX_THREAD               thread_232_1;
static TX_THREAD               thread_233_1;
static TX_THREAD               thread_234_1;
static TX_THREAD               thread_235_1;
static TX_THREAD               thread_236_1;
static TX_THREAD               thread_237_1;
static TX_THREAD               thread_238_1;
static TX_THREAD               thread_239_1;
static TX_THREAD               thread_240_1;
static TX_THREAD               thread_241_1;
static TX_THREAD               thread_242_1;
static TX_THREAD               thread_243_1;
static TX_THREAD               thread_244_1;
static TX_THREAD               thread_245_1;
static TX_THREAD               thread_246_1;
static TX_THREAD               thread_247_1;
static TX_THREAD               thread_248_1;
static TX_THREAD               thread_249_1;
static TX_THREAD               thread_250_1;
static TX_THREAD               thread_251_1;
static TX_THREAD               thread_252_1;
static TX_THREAD               thread_253_1;
static TX_THREAD               thread_254_1;
static TX_THREAD               thread_255_1;

static TX_THREAD               thread_0_2;
static TX_THREAD               thread_1_2;
static TX_THREAD               thread_2_2;
static TX_THREAD               thread_3_2;
static TX_THREAD               thread_4_2;
static TX_THREAD               thread_5_2;
static TX_THREAD               thread_6_2;
static TX_THREAD               thread_7_2;
static TX_THREAD               thread_8_2;
static TX_THREAD               thread_9_2;
static TX_THREAD               thread_10_2;
static TX_THREAD               thread_11_2;
static TX_THREAD               thread_12_2;
static TX_THREAD               thread_13_2;
static TX_THREAD               thread_14_2;
static TX_THREAD               thread_15_2;
static TX_THREAD               thread_16_2;
static TX_THREAD               thread_17_2;
static TX_THREAD               thread_18_2;
static TX_THREAD               thread_19_2;
static TX_THREAD               thread_20_2;
static TX_THREAD               thread_21_2;
static TX_THREAD               thread_22_2;
static TX_THREAD               thread_23_2;
static TX_THREAD               thread_24_2;
static TX_THREAD               thread_25_2;
static TX_THREAD               thread_26_2;
static TX_THREAD               thread_27_2;
static TX_THREAD               thread_28_2;
static TX_THREAD               thread_29_2;
static TX_THREAD               thread_30_2;
static TX_THREAD               thread_31_2;
static TX_THREAD               thread_32_2;
static TX_THREAD               thread_33_2;
static TX_THREAD               thread_34_2;
static TX_THREAD               thread_35_2;
static TX_THREAD               thread_36_2;
static TX_THREAD               thread_37_2;
static TX_THREAD               thread_38_2;
static TX_THREAD               thread_39_2;
static TX_THREAD               thread_40_2;
static TX_THREAD               thread_41_2;
static TX_THREAD               thread_42_2;
static TX_THREAD               thread_43_2;
static TX_THREAD               thread_44_2;
static TX_THREAD               thread_45_2;
static TX_THREAD               thread_46_2;
static TX_THREAD               thread_47_2;
static TX_THREAD               thread_48_2;
static TX_THREAD               thread_49_2;
static TX_THREAD               thread_50_2;
static TX_THREAD               thread_51_2;
static TX_THREAD               thread_52_2;
static TX_THREAD               thread_53_2;
static TX_THREAD               thread_54_2;
static TX_THREAD               thread_55_2;
static TX_THREAD               thread_56_2;
static TX_THREAD               thread_57_2;
static TX_THREAD               thread_58_2;
static TX_THREAD               thread_59_2;
static TX_THREAD               thread_60_2;
static TX_THREAD               thread_61_2;
static TX_THREAD               thread_62_2;
static TX_THREAD               thread_63_2;
static TX_THREAD               thread_64_2;
static TX_THREAD               thread_65_2;
static TX_THREAD               thread_66_2;
static TX_THREAD               thread_67_2;
static TX_THREAD               thread_68_2;
static TX_THREAD               thread_69_2;
static TX_THREAD               thread_70_2;
static TX_THREAD               thread_71_2;
static TX_THREAD               thread_72_2;
static TX_THREAD               thread_73_2;
static TX_THREAD               thread_74_2;
static TX_THREAD               thread_75_2;
static TX_THREAD               thread_76_2;
static TX_THREAD               thread_77_2;
static TX_THREAD               thread_78_2;
static TX_THREAD               thread_79_2;
static TX_THREAD               thread_80_2;
static TX_THREAD               thread_81_2;
static TX_THREAD               thread_82_2;
static TX_THREAD               thread_83_2;
static TX_THREAD               thread_84_2;
static TX_THREAD               thread_85_2;
static TX_THREAD               thread_86_2;
static TX_THREAD               thread_87_2;
static TX_THREAD               thread_88_2;
static TX_THREAD               thread_89_2;
static TX_THREAD               thread_90_2;
static TX_THREAD               thread_91_2;
static TX_THREAD               thread_92_2;
static TX_THREAD               thread_93_2;
static TX_THREAD               thread_94_2;
static TX_THREAD               thread_95_2;
static TX_THREAD               thread_96_2;
static TX_THREAD               thread_97_2;
static TX_THREAD               thread_98_2;
static TX_THREAD               thread_99_2;
static TX_THREAD               thread_100_2;
static TX_THREAD               thread_101_2;
static TX_THREAD               thread_102_2;
static TX_THREAD               thread_103_2;
static TX_THREAD               thread_104_2;
static TX_THREAD               thread_105_2;
static TX_THREAD               thread_106_2;
static TX_THREAD               thread_107_2;
static TX_THREAD               thread_108_2;
static TX_THREAD               thread_109_2;
static TX_THREAD               thread_110_2;
static TX_THREAD               thread_111_2;
static TX_THREAD               thread_112_2;
static TX_THREAD               thread_113_2;
static TX_THREAD               thread_114_2;
static TX_THREAD               thread_115_2;
static TX_THREAD               thread_116_2;
static TX_THREAD               thread_117_2;
static TX_THREAD               thread_118_2;
static TX_THREAD               thread_119_2;
static TX_THREAD               thread_120_2;
static TX_THREAD               thread_121_2;
static TX_THREAD               thread_122_2;
static TX_THREAD               thread_123_2;
static TX_THREAD               thread_124_2;
static TX_THREAD               thread_125_2;
static TX_THREAD               thread_126_2;
static TX_THREAD               thread_127_2;
static TX_THREAD               thread_128_2;
static TX_THREAD               thread_129_2;
static TX_THREAD               thread_130_2;
static TX_THREAD               thread_131_2;
static TX_THREAD               thread_132_2;
static TX_THREAD               thread_133_2;
static TX_THREAD               thread_134_2;
static TX_THREAD               thread_135_2;
static TX_THREAD               thread_136_2;
static TX_THREAD               thread_137_2;
static TX_THREAD               thread_138_2;
static TX_THREAD               thread_139_2;
static TX_THREAD               thread_140_2;
static TX_THREAD               thread_141_2;
static TX_THREAD               thread_142_2;
static TX_THREAD               thread_143_2;
static TX_THREAD               thread_144_2;
static TX_THREAD               thread_145_2;
static TX_THREAD               thread_146_2;
static TX_THREAD               thread_147_2;
static TX_THREAD               thread_148_2;
static TX_THREAD               thread_149_2;
static TX_THREAD               thread_150_2;
static TX_THREAD               thread_151_2;
static TX_THREAD               thread_152_2;
static TX_THREAD               thread_153_2;
static TX_THREAD               thread_154_2;
static TX_THREAD               thread_155_2;
static TX_THREAD               thread_156_2;
static TX_THREAD               thread_157_2;
static TX_THREAD               thread_158_2;
static TX_THREAD               thread_159_2;
static TX_THREAD               thread_160_2;
static TX_THREAD               thread_161_2;
static TX_THREAD               thread_162_2;
static TX_THREAD               thread_163_2;
static TX_THREAD               thread_164_2;
static TX_THREAD               thread_165_2;
static TX_THREAD               thread_166_2;
static TX_THREAD               thread_167_2;
static TX_THREAD               thread_168_2;
static TX_THREAD               thread_169_2;
static TX_THREAD               thread_170_2;
static TX_THREAD               thread_171_2;
static TX_THREAD               thread_172_2;
static TX_THREAD               thread_173_2;
static TX_THREAD               thread_174_2;
static TX_THREAD               thread_175_2;
static TX_THREAD               thread_176_2;
static TX_THREAD               thread_177_2;
static TX_THREAD               thread_178_2;
static TX_THREAD               thread_179_2;
static TX_THREAD               thread_180_2;
static TX_THREAD               thread_181_2;
static TX_THREAD               thread_182_2;
static TX_THREAD               thread_183_2;
static TX_THREAD               thread_184_2;
static TX_THREAD               thread_185_2;
static TX_THREAD               thread_186_2;
static TX_THREAD               thread_187_2;
static TX_THREAD               thread_188_2;
static TX_THREAD               thread_189_2;
static TX_THREAD               thread_190_2;
static TX_THREAD               thread_191_2;
static TX_THREAD               thread_192_2;
static TX_THREAD               thread_193_2;
static TX_THREAD               thread_194_2;
static TX_THREAD               thread_195_2;
static TX_THREAD               thread_196_2;
static TX_THREAD               thread_197_2;
static TX_THREAD               thread_198_2;
static TX_THREAD               thread_199_2;
static TX_THREAD               thread_200_2;
static TX_THREAD               thread_201_2;
static TX_THREAD               thread_202_2;
static TX_THREAD               thread_203_2;
static TX_THREAD               thread_204_2;
static TX_THREAD               thread_205_2;
static TX_THREAD               thread_206_2;
static TX_THREAD               thread_207_2;
static TX_THREAD               thread_208_2;
static TX_THREAD               thread_209_2;
static TX_THREAD               thread_210_2;
static TX_THREAD               thread_211_2;
static TX_THREAD               thread_212_2;
static TX_THREAD               thread_213_2;
static TX_THREAD               thread_214_2;
static TX_THREAD               thread_215_2;
static TX_THREAD               thread_216_2;
static TX_THREAD               thread_217_2;
static TX_THREAD               thread_218_2;
static TX_THREAD               thread_219_2;
static TX_THREAD               thread_220_2;
static TX_THREAD               thread_221_2;
static TX_THREAD               thread_222_2;
static TX_THREAD               thread_223_2;
static TX_THREAD               thread_224_2;
static TX_THREAD               thread_225_2;
static TX_THREAD               thread_226_2;
static TX_THREAD               thread_227_2;
static TX_THREAD               thread_228_2;
static TX_THREAD               thread_229_2;
static TX_THREAD               thread_230_2;
static TX_THREAD               thread_231_2;
static TX_THREAD               thread_232_2;
static TX_THREAD               thread_233_2;
static TX_THREAD               thread_234_2;
static TX_THREAD               thread_235_2;
static TX_THREAD               thread_236_2;
static TX_THREAD               thread_237_2;
static TX_THREAD               thread_238_2;
static TX_THREAD               thread_239_2;
static TX_THREAD               thread_240_2;
static TX_THREAD               thread_241_2;
static TX_THREAD               thread_242_2;
static TX_THREAD               thread_243_2;
static TX_THREAD               thread_244_2;
static TX_THREAD               thread_245_2;
static TX_THREAD               thread_246_2;
static TX_THREAD               thread_247_2;
static TX_THREAD               thread_248_2;
static TX_THREAD               thread_249_2;
static TX_THREAD               thread_250_2;
static TX_THREAD               thread_251_2;
static TX_THREAD               thread_252_2;
static TX_THREAD               thread_253_2;
static TX_THREAD               thread_254_2;
static TX_THREAD               thread_255_2;

static TX_THREAD               thread_0_3;
static TX_THREAD               thread_1_3;
static TX_THREAD               thread_2_3;
static TX_THREAD               thread_3_3;
static TX_THREAD               thread_4_3;
static TX_THREAD               thread_5_3;
static TX_THREAD               thread_6_3;
static TX_THREAD               thread_7_3;
static TX_THREAD               thread_8_3;
static TX_THREAD               thread_9_3;
static TX_THREAD               thread_10_3;
static TX_THREAD               thread_11_3;
static TX_THREAD               thread_12_3;
static TX_THREAD               thread_13_3;
static TX_THREAD               thread_14_3;
static TX_THREAD               thread_15_3;
static TX_THREAD               thread_16_3;
static TX_THREAD               thread_17_3;
static TX_THREAD               thread_18_3;
static TX_THREAD               thread_19_3;
static TX_THREAD               thread_20_3;
static TX_THREAD               thread_21_3;
static TX_THREAD               thread_22_3;
static TX_THREAD               thread_23_3;
static TX_THREAD               thread_24_3;
static TX_THREAD               thread_25_3;
static TX_THREAD               thread_26_3;
static TX_THREAD               thread_27_3;
static TX_THREAD               thread_28_3;
static TX_THREAD               thread_29_3;
static TX_THREAD               thread_30_3;
static TX_THREAD               thread_31_3;
static TX_THREAD               thread_32_3;
static TX_THREAD               thread_33_3;
static TX_THREAD               thread_34_3;
static TX_THREAD               thread_35_3;
static TX_THREAD               thread_36_3;
static TX_THREAD               thread_37_3;
static TX_THREAD               thread_38_3;
static TX_THREAD               thread_39_3;
static TX_THREAD               thread_40_3;
static TX_THREAD               thread_41_3;
static TX_THREAD               thread_42_3;
static TX_THREAD               thread_43_3;
static TX_THREAD               thread_44_3;
static TX_THREAD               thread_45_3;
static TX_THREAD               thread_46_3;
static TX_THREAD               thread_47_3;
static TX_THREAD               thread_48_3;
static TX_THREAD               thread_49_3;
static TX_THREAD               thread_50_3;
static TX_THREAD               thread_51_3;
static TX_THREAD               thread_52_3;
static TX_THREAD               thread_53_3;
static TX_THREAD               thread_54_3;
static TX_THREAD               thread_55_3;
static TX_THREAD               thread_56_3;
static TX_THREAD               thread_57_3;
static TX_THREAD               thread_58_3;
static TX_THREAD               thread_59_3;
static TX_THREAD               thread_60_3;
static TX_THREAD               thread_61_3;
static TX_THREAD               thread_62_3;
static TX_THREAD               thread_63_3;
static TX_THREAD               thread_64_3;
static TX_THREAD               thread_65_3;
static TX_THREAD               thread_66_3;
static TX_THREAD               thread_67_3;
static TX_THREAD               thread_68_3;
static TX_THREAD               thread_69_3;
static TX_THREAD               thread_70_3;
static TX_THREAD               thread_71_3;
static TX_THREAD               thread_72_3;
static TX_THREAD               thread_73_3;
static TX_THREAD               thread_74_3;
static TX_THREAD               thread_75_3;
static TX_THREAD               thread_76_3;
static TX_THREAD               thread_77_3;
static TX_THREAD               thread_78_3;
static TX_THREAD               thread_79_3;
static TX_THREAD               thread_80_3;
static TX_THREAD               thread_81_3;
static TX_THREAD               thread_82_3;
static TX_THREAD               thread_83_3;
static TX_THREAD               thread_84_3;
static TX_THREAD               thread_85_3;
static TX_THREAD               thread_86_3;
static TX_THREAD               thread_87_3;
static TX_THREAD               thread_88_3;
static TX_THREAD               thread_89_3;
static TX_THREAD               thread_90_3;
static TX_THREAD               thread_91_3;
static TX_THREAD               thread_92_3;
static TX_THREAD               thread_93_3;
static TX_THREAD               thread_94_3;
static TX_THREAD               thread_95_3;
static TX_THREAD               thread_96_3;
static TX_THREAD               thread_97_3;
static TX_THREAD               thread_98_3;
static TX_THREAD               thread_99_3;
static TX_THREAD               thread_100_3;
static TX_THREAD               thread_101_3;
static TX_THREAD               thread_102_3;
static TX_THREAD               thread_103_3;
static TX_THREAD               thread_104_3;
static TX_THREAD               thread_105_3;
static TX_THREAD               thread_106_3;
static TX_THREAD               thread_107_3;
static TX_THREAD               thread_108_3;
static TX_THREAD               thread_109_3;
static TX_THREAD               thread_110_3;
static TX_THREAD               thread_111_3;
static TX_THREAD               thread_112_3;
static TX_THREAD               thread_113_3;
static TX_THREAD               thread_114_3;
static TX_THREAD               thread_115_3;
static TX_THREAD               thread_116_3;
static TX_THREAD               thread_117_3;
static TX_THREAD               thread_118_3;
static TX_THREAD               thread_119_3;
static TX_THREAD               thread_120_3;
static TX_THREAD               thread_121_3;
static TX_THREAD               thread_122_3;
static TX_THREAD               thread_123_3;
static TX_THREAD               thread_124_3;
static TX_THREAD               thread_125_3;
static TX_THREAD               thread_126_3;
static TX_THREAD               thread_127_3;
static TX_THREAD               thread_128_3;
static TX_THREAD               thread_129_3;
static TX_THREAD               thread_130_3;
static TX_THREAD               thread_131_3;
static TX_THREAD               thread_132_3;
static TX_THREAD               thread_133_3;
static TX_THREAD               thread_134_3;
static TX_THREAD               thread_135_3;
static TX_THREAD               thread_136_3;
static TX_THREAD               thread_137_3;
static TX_THREAD               thread_138_3;
static TX_THREAD               thread_139_3;
static TX_THREAD               thread_140_3;
static TX_THREAD               thread_141_3;
static TX_THREAD               thread_142_3;
static TX_THREAD               thread_143_3;
static TX_THREAD               thread_144_3;
static TX_THREAD               thread_145_3;
static TX_THREAD               thread_146_3;
static TX_THREAD               thread_147_3;
static TX_THREAD               thread_148_3;
static TX_THREAD               thread_149_3;
static TX_THREAD               thread_150_3;
static TX_THREAD               thread_151_3;
static TX_THREAD               thread_152_3;
static TX_THREAD               thread_153_3;
static TX_THREAD               thread_154_3;
static TX_THREAD               thread_155_3;
static TX_THREAD               thread_156_3;
static TX_THREAD               thread_157_3;
static TX_THREAD               thread_158_3;
static TX_THREAD               thread_159_3;
static TX_THREAD               thread_160_3;
static TX_THREAD               thread_161_3;
static TX_THREAD               thread_162_3;
static TX_THREAD               thread_163_3;
static TX_THREAD               thread_164_3;
static TX_THREAD               thread_165_3;
static TX_THREAD               thread_166_3;
static TX_THREAD               thread_167_3;
static TX_THREAD               thread_168_3;
static TX_THREAD               thread_169_3;
static TX_THREAD               thread_170_3;
static TX_THREAD               thread_171_3;
static TX_THREAD               thread_172_3;
static TX_THREAD               thread_173_3;
static TX_THREAD               thread_174_3;
static TX_THREAD               thread_175_3;
static TX_THREAD               thread_176_3;
static TX_THREAD               thread_177_3;
static TX_THREAD               thread_178_3;
static TX_THREAD               thread_179_3;
static TX_THREAD               thread_180_3;
static TX_THREAD               thread_181_3;
static TX_THREAD               thread_182_3;
static TX_THREAD               thread_183_3;
static TX_THREAD               thread_184_3;
static TX_THREAD               thread_185_3;
static TX_THREAD               thread_186_3;
static TX_THREAD               thread_187_3;
static TX_THREAD               thread_188_3;
static TX_THREAD               thread_189_3;
static TX_THREAD               thread_190_3;
static TX_THREAD               thread_191_3;
static TX_THREAD               thread_192_3;
static TX_THREAD               thread_193_3;
static TX_THREAD               thread_194_3;
static TX_THREAD               thread_195_3;
static TX_THREAD               thread_196_3;
static TX_THREAD               thread_197_3;
static TX_THREAD               thread_198_3;
static TX_THREAD               thread_199_3;
static TX_THREAD               thread_200_3;
static TX_THREAD               thread_201_3;
static TX_THREAD               thread_202_3;
static TX_THREAD               thread_203_3;
static TX_THREAD               thread_204_3;
static TX_THREAD               thread_205_3;
static TX_THREAD               thread_206_3;
static TX_THREAD               thread_207_3;
static TX_THREAD               thread_208_3;
static TX_THREAD               thread_209_3;
static TX_THREAD               thread_210_3;
static TX_THREAD               thread_211_3;
static TX_THREAD               thread_212_3;
static TX_THREAD               thread_213_3;
static TX_THREAD               thread_214_3;
static TX_THREAD               thread_215_3;
static TX_THREAD               thread_216_3;
static TX_THREAD               thread_217_3;
static TX_THREAD               thread_218_3;
static TX_THREAD               thread_219_3;
static TX_THREAD               thread_220_3;
static TX_THREAD               thread_221_3;
static TX_THREAD               thread_222_3;
static TX_THREAD               thread_223_3;
static TX_THREAD               thread_224_3;
static TX_THREAD               thread_225_3;
static TX_THREAD               thread_226_3;
static TX_THREAD               thread_227_3;
static TX_THREAD               thread_228_3;
static TX_THREAD               thread_229_3;
static TX_THREAD               thread_230_3;
static TX_THREAD               thread_231_3;
static TX_THREAD               thread_232_3;
static TX_THREAD               thread_233_3;
static TX_THREAD               thread_234_3;
static TX_THREAD               thread_235_3;
static TX_THREAD               thread_236_3;
static TX_THREAD               thread_237_3;
static TX_THREAD               thread_238_3;
static TX_THREAD               thread_239_3;
static TX_THREAD               thread_240_3;
static TX_THREAD               thread_241_3;
static TX_THREAD               thread_242_3;
static TX_THREAD               thread_243_3;
static TX_THREAD               thread_244_3;
static TX_THREAD               thread_245_3;
static TX_THREAD               thread_246_3;
static TX_THREAD               thread_247_3;
static TX_THREAD               thread_248_3;
static TX_THREAD               thread_249_3;
static TX_THREAD               thread_250_3;
static TX_THREAD               thread_251_3;
static TX_THREAD               thread_252_3;
static TX_THREAD               thread_253_3;
static TX_THREAD               thread_254_3;
static TX_THREAD               thread_255_3;

/* Define test array.  */

static TX_THREAD   *_smp_randomized_source_array[] = {
{&thread_0},
{&thread_1},
{&thread_2},
{&thread_3},
{&thread_4},
{&thread_5},
{&thread_6},
{&thread_7},
{&thread_8},
{&thread_9},
{&thread_10},
{&thread_11},
{&thread_12},
{&thread_13},
{&thread_14},
{&thread_15},
{&thread_16},
{&thread_17},
{&thread_18},
{&thread_19},
{&thread_20},
{&thread_21},
{&thread_22},
{&thread_23},
{&thread_24},
{&thread_25},
{&thread_26},
{&thread_27},
{&thread_28},
{&thread_29},
{&thread_30},
{&thread_31},
{&thread_32},
{&thread_33},
{&thread_34},
{&thread_35},
{&thread_36},
{&thread_37},
{&thread_38},
{&thread_39},
{&thread_40},
{&thread_41},
{&thread_42},
{&thread_43},
{&thread_44},
{&thread_45},
{&thread_46},
{&thread_47},
{&thread_48},
{&thread_49},
{&thread_50},
{&thread_51},
{&thread_52},
{&thread_53},
{&thread_54},
{&thread_55},
{&thread_56},
{&thread_57},
{&thread_58},
{&thread_59},
{&thread_60},
{&thread_61},
{&thread_62},
{&thread_63},
{&thread_64},
{&thread_65},
{&thread_66},
{&thread_67},
{&thread_68},
{&thread_69},
{&thread_70},
{&thread_71},
{&thread_72},
{&thread_73},
{&thread_74},
{&thread_75},
{&thread_76},
{&thread_77},
{&thread_78},
{&thread_79},
{&thread_80},
{&thread_81},
{&thread_82},
{&thread_83},
{&thread_84},
{&thread_85},
{&thread_86},
{&thread_87},
{&thread_88},
{&thread_89},
{&thread_90},
{&thread_91},
{&thread_92},
{&thread_93},
{&thread_94},
{&thread_95},
{&thread_96},
{&thread_97},
{&thread_98},
{&thread_99},
{&thread_100},
{&thread_101},
{&thread_102},
{&thread_103},
{&thread_104},
{&thread_105},
{&thread_106},
{&thread_107},
{&thread_108},
{&thread_109},
{&thread_110},
{&thread_111},
{&thread_112},
{&thread_113},
{&thread_114},
{&thread_115},
{&thread_116},
{&thread_117},
{&thread_118},
{&thread_119},
{&thread_120},
{&thread_121},
{&thread_122},
{&thread_123},
{&thread_124},
{&thread_125},
{&thread_126},
{&thread_127},
{&thread_128},
{&thread_129},
{&thread_130},
{&thread_131},
{&thread_132},
{&thread_133},
{&thread_134},
{&thread_135},
{&thread_136},
{&thread_137},
{&thread_138},
{&thread_139},
{&thread_140},
{&thread_141},
{&thread_142},
{&thread_143},
{&thread_144},
{&thread_145},
{&thread_146},
{&thread_147},
{&thread_148},
{&thread_149},
{&thread_150},
{&thread_151},
{&thread_152},
{&thread_153},
{&thread_154},
{&thread_155},
{&thread_156},
{&thread_157},
{&thread_158},
{&thread_159},
{&thread_160},
{&thread_161},
{&thread_162},
{&thread_163},
{&thread_164},
{&thread_165},
{&thread_166},
{&thread_167},
{&thread_168},
{&thread_169},
{&thread_170},
{&thread_171},
{&thread_172},
{&thread_173},
{&thread_174},
{&thread_175},
{&thread_176},
{&thread_177},
{&thread_178},
{&thread_179},
{&thread_180},
{&thread_181},
{&thread_182},
{&thread_183},
{&thread_184},
{&thread_185},
{&thread_186},
{&thread_187},
{&thread_188},
{&thread_189},
{&thread_190},
{&thread_191},
{&thread_192},
{&thread_193},
{&thread_194},
{&thread_195},
{&thread_196},
{&thread_197},
{&thread_198},
{&thread_199},
{&thread_200},
{&thread_201},
{&thread_202},
{&thread_203},
{&thread_204},
{&thread_205},
{&thread_206},
{&thread_207},
{&thread_208},
{&thread_209},
{&thread_210},
{&thread_211},
{&thread_212},
{&thread_213},
{&thread_214},
{&thread_215},
{&thread_216},
{&thread_217},
{&thread_218},
{&thread_219},
{&thread_220},
{&thread_221},
{&thread_222},
{&thread_223},
{&thread_224},
{&thread_225},
{&thread_226},
{&thread_227},
{&thread_228},
{&thread_229},
{&thread_230},
{&thread_231},
{&thread_232},
{&thread_233},
{&thread_234},
{&thread_235},
{&thread_236},
{&thread_237},
{&thread_238},
{&thread_239},
{&thread_240},
{&thread_241},
{&thread_242},
{&thread_243},
{&thread_244},
{&thread_245},
{&thread_246},
{&thread_247},
{&thread_248},
{&thread_249},
{&thread_250},
{&thread_251},
{&thread_252},
{&thread_253},
{&thread_254},
{&thread_255},

{&thread_0_1},
{&thread_1_1},
{&thread_2_1},
{&thread_3_1},
{&thread_4_1},
{&thread_5_1},
{&thread_6_1},
{&thread_7_1},
{&thread_8_1},
{&thread_9_1},
{&thread_10_1},
{&thread_11_1},
{&thread_12_1},
{&thread_13_1},
{&thread_14_1},
{&thread_15_1},
{&thread_16_1},
{&thread_17_1},
{&thread_18_1},
{&thread_19_1},
{&thread_20_1},
{&thread_21_1},
{&thread_22_1},
{&thread_23_1},
{&thread_24_1},
{&thread_25_1},
{&thread_26_1},
{&thread_27_1},
{&thread_28_1},
{&thread_29_1},
{&thread_30_1},
{&thread_31_1},
{&thread_32_1},
{&thread_33_1},
{&thread_34_1},
{&thread_35_1},
{&thread_36_1},
{&thread_37_1},
{&thread_38_1},
{&thread_39_1},
{&thread_40_1},
{&thread_41_1},
{&thread_42_1},
{&thread_43_1},
{&thread_44_1},
{&thread_45_1},
{&thread_46_1},
{&thread_47_1},
{&thread_48_1},
{&thread_49_1},
{&thread_50_1},
{&thread_51_1},
{&thread_52_1},
{&thread_53_1},
{&thread_54_1},
{&thread_55_1},
{&thread_56_1},
{&thread_57_1},
{&thread_58_1},
{&thread_59_1},
{&thread_60_1},
{&thread_61_1},
{&thread_62_1},
{&thread_63_1},
{&thread_64_1},
{&thread_65_1},
{&thread_66_1},
{&thread_67_1},
{&thread_68_1},
{&thread_69_1},
{&thread_70_1},
{&thread_71_1},
{&thread_72_1},
{&thread_73_1},
{&thread_74_1},
{&thread_75_1},
{&thread_76_1},
{&thread_77_1},
{&thread_78_1},
{&thread_79_1},
{&thread_80_1},
{&thread_81_1},
{&thread_82_1},
{&thread_83_1},
{&thread_84_1},
{&thread_85_1},
{&thread_86_1},
{&thread_87_1},
{&thread_88_1},
{&thread_89_1},
{&thread_90_1},
{&thread_91_1},
{&thread_92_1},
{&thread_93_1},
{&thread_94_1},
{&thread_95_1},
{&thread_96_1},
{&thread_97_1},
{&thread_98_1},
{&thread_99_1},
{&thread_100_1},
{&thread_101_1},
{&thread_102_1},
{&thread_103_1},
{&thread_104_1},
{&thread_105_1},
{&thread_106_1},
{&thread_107_1},
{&thread_108_1},
{&thread_109_1},
{&thread_110_1},
{&thread_111_1},
{&thread_112_1},
{&thread_113_1},
{&thread_114_1},
{&thread_115_1},
{&thread_116_1},
{&thread_117_1},
{&thread_118_1},
{&thread_119_1},
{&thread_120_1},
{&thread_121_1},
{&thread_122_1},
{&thread_123_1},
{&thread_124_1},
{&thread_125_1},
{&thread_126_1},
{&thread_127_1},
{&thread_128_1},
{&thread_129_1},
{&thread_130_1},
{&thread_131_1},
{&thread_132_1},
{&thread_133_1},
{&thread_134_1},
{&thread_135_1},
{&thread_136_1},
{&thread_137_1},
{&thread_138_1},
{&thread_139_1},
{&thread_140_1},
{&thread_141_1},
{&thread_142_1},
{&thread_143_1},
{&thread_144_1},
{&thread_145_1},
{&thread_146_1},
{&thread_147_1},
{&thread_148_1},
{&thread_149_1},
{&thread_150_1},
{&thread_151_1},
{&thread_152_1},
{&thread_153_1},
{&thread_154_1},
{&thread_155_1},
{&thread_156_1},
{&thread_157_1},
{&thread_158_1},
{&thread_159_1},
{&thread_160_1},
{&thread_161_1},
{&thread_162_1},
{&thread_163_1},
{&thread_164_1},
{&thread_165_1},
{&thread_166_1},
{&thread_167_1},
{&thread_168_1},
{&thread_169_1},
{&thread_170_1},
{&thread_171_1},
{&thread_172_1},
{&thread_173_1},
{&thread_174_1},
{&thread_175_1},
{&thread_176_1},
{&thread_177_1},
{&thread_178_1},
{&thread_179_1},
{&thread_180_1},
{&thread_181_1},
{&thread_182_1},
{&thread_183_1},
{&thread_184_1},
{&thread_185_1},
{&thread_186_1},
{&thread_187_1},
{&thread_188_1},
{&thread_189_1},
{&thread_190_1},
{&thread_191_1},
{&thread_192_1},
{&thread_193_1},
{&thread_194_1},
{&thread_195_1},
{&thread_196_1},
{&thread_197_1},
{&thread_198_1},
{&thread_199_1},
{&thread_200_1},
{&thread_201_1},
{&thread_202_1},
{&thread_203_1},
{&thread_204_1},
{&thread_205_1},
{&thread_206_1},
{&thread_207_1},
{&thread_208_1},
{&thread_209_1},
{&thread_210_1},
{&thread_211_1},
{&thread_212_1},
{&thread_213_1},
{&thread_214_1},
{&thread_215_1},
{&thread_216_1},
{&thread_217_1},
{&thread_218_1},
{&thread_219_1},
{&thread_220_1},
{&thread_221_1},
{&thread_222_1},
{&thread_223_1},
{&thread_224_1},
{&thread_225_1},
{&thread_226_1},
{&thread_227_1},
{&thread_228_1},
{&thread_229_1},
{&thread_230_1},
{&thread_231_1},
{&thread_232_1},
{&thread_233_1},
{&thread_234_1},
{&thread_235_1},
{&thread_236_1},
{&thread_237_1},
{&thread_238_1},
{&thread_239_1},
{&thread_240_1},
{&thread_241_1},
{&thread_242_1},
{&thread_243_1},
{&thread_244_1},
{&thread_245_1},
{&thread_246_1},
{&thread_247_1},
{&thread_248_1},
{&thread_249_1},
{&thread_250_1},
{&thread_251_1},
{&thread_252_1},
{&thread_253_1},
{&thread_254_1},
{&thread_255_1},

{&thread_0_2},
{&thread_1_2},
{&thread_2_2},
{&thread_3_2},
{&thread_4_2},
{&thread_5_2},
{&thread_6_2},
{&thread_7_2},
{&thread_8_2},
{&thread_9_2},
{&thread_10_2},
{&thread_11_2},
{&thread_12_2},
{&thread_13_2},
{&thread_14_2},
{&thread_15_2},
{&thread_16_2},
{&thread_17_2},
{&thread_18_2},
{&thread_19_2},
{&thread_20_2},
{&thread_21_2},
{&thread_22_2},
{&thread_23_2},
{&thread_24_2},
{&thread_25_2},
{&thread_26_2},
{&thread_27_2},
{&thread_28_2},
{&thread_29_2},
{&thread_30_2},
{&thread_31_2},
{&thread_32_2},
{&thread_33_2},
{&thread_34_2},
{&thread_35_2},
{&thread_36_2},
{&thread_37_2},
{&thread_38_2},
{&thread_39_2},
{&thread_40_2},
{&thread_41_2},
{&thread_42_2},
{&thread_43_2},
{&thread_44_2},
{&thread_45_2},
{&thread_46_2},
{&thread_47_2},
{&thread_48_2},
{&thread_49_2},
{&thread_50_2},
{&thread_51_2},
{&thread_52_2},
{&thread_53_2},
{&thread_54_2},
{&thread_55_2},
{&thread_56_2},
{&thread_57_2},
{&thread_58_2},
{&thread_59_2},
{&thread_60_2},
{&thread_61_2},
{&thread_62_2},
{&thread_63_2},
{&thread_64_2},
{&thread_65_2},
{&thread_66_2},
{&thread_67_2},
{&thread_68_2},
{&thread_69_2},
{&thread_70_2},
{&thread_71_2},
{&thread_72_2},
{&thread_73_2},
{&thread_74_2},
{&thread_75_2},
{&thread_76_2},
{&thread_77_2},
{&thread_78_2},
{&thread_79_2},
{&thread_80_2},
{&thread_81_2},
{&thread_82_2},
{&thread_83_2},
{&thread_84_2},
{&thread_85_2},
{&thread_86_2},
{&thread_87_2},
{&thread_88_2},
{&thread_89_2},
{&thread_90_2},
{&thread_91_2},
{&thread_92_2},
{&thread_93_2},
{&thread_94_2},
{&thread_95_2},
{&thread_96_2},
{&thread_97_2},
{&thread_98_2},
{&thread_99_2},
{&thread_100_2},
{&thread_101_2},
{&thread_102_2},
{&thread_103_2},
{&thread_104_2},
{&thread_105_2},
{&thread_106_2},
{&thread_107_2},
{&thread_108_2},
{&thread_109_2},
{&thread_110_2},
{&thread_111_2},
{&thread_112_2},
{&thread_113_2},
{&thread_114_2},
{&thread_115_2},
{&thread_116_2},
{&thread_117_2},
{&thread_118_2},
{&thread_119_2},
{&thread_120_2},
{&thread_121_2},
{&thread_122_2},
{&thread_123_2},
{&thread_124_2},
{&thread_125_2},
{&thread_126_2},
{&thread_127_2},
{&thread_128_2},
{&thread_129_2},
{&thread_130_2},
{&thread_131_2},
{&thread_132_2},
{&thread_133_2},
{&thread_134_2},
{&thread_135_2},
{&thread_136_2},
{&thread_137_2},
{&thread_138_2},
{&thread_139_2},
{&thread_140_2},
{&thread_141_2},
{&thread_142_2},
{&thread_143_2},
{&thread_144_2},
{&thread_145_2},
{&thread_146_2},
{&thread_147_2},
{&thread_148_2},
{&thread_149_2},
{&thread_150_2},
{&thread_151_2},
{&thread_152_2},
{&thread_153_2},
{&thread_154_2},
{&thread_155_2},
{&thread_156_2},
{&thread_157_2},
{&thread_158_2},
{&thread_159_2},
{&thread_160_2},
{&thread_161_2},
{&thread_162_2},
{&thread_163_2},
{&thread_164_2},
{&thread_165_2},
{&thread_166_2},
{&thread_167_2},
{&thread_168_2},
{&thread_169_2},
{&thread_170_2},
{&thread_171_2},
{&thread_172_2},
{&thread_173_2},
{&thread_174_2},
{&thread_175_2},
{&thread_176_2},
{&thread_177_2},
{&thread_178_2},
{&thread_179_2},
{&thread_180_2},
{&thread_181_2},
{&thread_182_2},
{&thread_183_2},
{&thread_184_2},
{&thread_185_2},
{&thread_186_2},
{&thread_187_2},
{&thread_188_2},
{&thread_189_2},
{&thread_190_2},
{&thread_191_2},
{&thread_192_2},
{&thread_193_2},
{&thread_194_2},
{&thread_195_2},
{&thread_196_2},
{&thread_197_2},
{&thread_198_2},
{&thread_199_2},
{&thread_200_2},
{&thread_201_2},
{&thread_202_2},
{&thread_203_2},
{&thread_204_2},
{&thread_205_2},
{&thread_206_2},
{&thread_207_2},
{&thread_208_2},
{&thread_209_2},
{&thread_210_2},
{&thread_211_2},
{&thread_212_2},
{&thread_213_2},
{&thread_214_2},
{&thread_215_2},
{&thread_216_2},
{&thread_217_2},
{&thread_218_2},
{&thread_219_2},
{&thread_220_2},
{&thread_221_2},
{&thread_222_2},
{&thread_223_2},
{&thread_224_2},
{&thread_225_2},
{&thread_226_2},
{&thread_227_2},
{&thread_228_2},
{&thread_229_2},
{&thread_230_2},
{&thread_231_2},
{&thread_232_2},
{&thread_233_2},
{&thread_234_2},
{&thread_235_2},
{&thread_236_2},
{&thread_237_2},
{&thread_238_2},
{&thread_239_2},
{&thread_240_2},
{&thread_241_2},
{&thread_242_2},
{&thread_243_2},
{&thread_244_2},
{&thread_245_2},
{&thread_246_2},
{&thread_247_2},
{&thread_248_2},
{&thread_249_2},
{&thread_250_2},
{&thread_251_2},
{&thread_252_2},
{&thread_253_2},
{&thread_254_2},
{&thread_255_2},

{&thread_0_3},
{&thread_1_3},
{&thread_2_3},
{&thread_3_3},
{&thread_4_3},
{&thread_5_3},
{&thread_6_3},
{&thread_7_3},
{&thread_8_3},
{&thread_9_3},
{&thread_10_3},
{&thread_11_3},
{&thread_12_3},
{&thread_13_3},
{&thread_14_3},
{&thread_15_3},
{&thread_16_3},
{&thread_17_3},
{&thread_18_3},
{&thread_19_3},
{&thread_20_3},
{&thread_21_3},
{&thread_22_3},
{&thread_23_3},
{&thread_24_3},
{&thread_25_3},
{&thread_26_3},
{&thread_27_3},
{&thread_28_3},
{&thread_29_3},
{&thread_30_3},
{&thread_31_3},
{&thread_32_3},
{&thread_33_3},
{&thread_34_3},
{&thread_35_3},
{&thread_36_3},
{&thread_37_3},
{&thread_38_3},
{&thread_39_3},
{&thread_40_3},
{&thread_41_3},
{&thread_42_3},
{&thread_43_3},
{&thread_44_3},
{&thread_45_3},
{&thread_46_3},
{&thread_47_3},
{&thread_48_3},
{&thread_49_3},
{&thread_50_3},
{&thread_51_3},
{&thread_52_3},
{&thread_53_3},
{&thread_54_3},
{&thread_55_3},
{&thread_56_3},
{&thread_57_3},
{&thread_58_3},
{&thread_59_3},
{&thread_60_3},
{&thread_61_3},
{&thread_62_3},
{&thread_63_3},
{&thread_64_3},
{&thread_65_3},
{&thread_66_3},
{&thread_67_3},
{&thread_68_3},
{&thread_69_3},
{&thread_70_3},
{&thread_71_3},
{&thread_72_3},
{&thread_73_3},
{&thread_74_3},
{&thread_75_3},
{&thread_76_3},
{&thread_77_3},
{&thread_78_3},
{&thread_79_3},
{&thread_80_3},
{&thread_81_3},
{&thread_82_3},
{&thread_83_3},
{&thread_84_3},
{&thread_85_3},
{&thread_86_3},
{&thread_87_3},
{&thread_88_3},
{&thread_89_3},
{&thread_90_3},
{&thread_91_3},
{&thread_92_3},
{&thread_93_3},
{&thread_94_3},
{&thread_95_3},
{&thread_96_3},
{&thread_97_3},
{&thread_98_3},
{&thread_99_3},
{&thread_100_3},
{&thread_101_3},
{&thread_102_3},
{&thread_103_3},
{&thread_104_3},
{&thread_105_3},
{&thread_106_3},
{&thread_107_3},
{&thread_108_3},
{&thread_109_3},
{&thread_110_3},
{&thread_111_3},
{&thread_112_3},
{&thread_113_3},
{&thread_114_3},
{&thread_115_3},
{&thread_116_3},
{&thread_117_3},
{&thread_118_3},
{&thread_119_3},
{&thread_120_3},
{&thread_121_3},
{&thread_122_3},
{&thread_123_3},
{&thread_124_3},
{&thread_125_3},
{&thread_126_3},
{&thread_127_3},
{&thread_128_3},
{&thread_129_3},
{&thread_130_3},
{&thread_131_3},
{&thread_132_3},
{&thread_133_3},
{&thread_134_3},
{&thread_135_3},
{&thread_136_3},
{&thread_137_3},
{&thread_138_3},
{&thread_139_3},
{&thread_140_3},
{&thread_141_3},
{&thread_142_3},
{&thread_143_3},
{&thread_144_3},
{&thread_145_3},
{&thread_146_3},
{&thread_147_3},
{&thread_148_3},
{&thread_149_3},
{&thread_150_3},
{&thread_151_3},
{&thread_152_3},
{&thread_153_3},
{&thread_154_3},
{&thread_155_3},
{&thread_156_3},
{&thread_157_3},
{&thread_158_3},
{&thread_159_3},
{&thread_160_3},
{&thread_161_3},
{&thread_162_3},
{&thread_163_3},
{&thread_164_3},
{&thread_165_3},
{&thread_166_3},
{&thread_167_3},
{&thread_168_3},
{&thread_169_3},
{&thread_170_3},
{&thread_171_3},
{&thread_172_3},
{&thread_173_3},
{&thread_174_3},
{&thread_175_3},
{&thread_176_3},
{&thread_177_3},
{&thread_178_3},
{&thread_179_3},
{&thread_180_3},
{&thread_181_3},
{&thread_182_3},
{&thread_183_3},
{&thread_184_3},
{&thread_185_3},
{&thread_186_3},
{&thread_187_3},
{&thread_188_3},
{&thread_189_3},
{&thread_190_3},
{&thread_191_3},
{&thread_192_3},
{&thread_193_3},
{&thread_194_3},
{&thread_195_3},
{&thread_196_3},
{&thread_197_3},
{&thread_198_3},
{&thread_199_3},
{&thread_200_3},
{&thread_201_3},
{&thread_202_3},
{&thread_203_3},
{&thread_204_3},
{&thread_205_3},
{&thread_206_3},
{&thread_207_3},
{&thread_208_3},
{&thread_209_3},
{&thread_210_3},
{&thread_211_3},
{&thread_212_3},
{&thread_213_3},
{&thread_214_3},
{&thread_215_3},
{&thread_216_3},
{&thread_217_3},
{&thread_218_3},
{&thread_219_3},
{&thread_220_3},
{&thread_221_3},
{&thread_222_3},
{&thread_223_3},
{&thread_224_3},
{&thread_225_3},
{&thread_226_3},
{&thread_227_3},
{&thread_228_3},
{&thread_229_3},
{&thread_230_3},
{&thread_231_3},
{&thread_232_3},
{&thread_233_3},
{&thread_234_3},
{&thread_235_3},
{&thread_236_3},
{&thread_237_3},
{&thread_238_3},
{&thread_239_3},
{&thread_240_3},
{&thread_241_3},
{&thread_242_3},
{&thread_243_3},
{&thread_244_3},
{&thread_245_3},
{&thread_246_3},
{&thread_247_3},
{&thread_248_3},
{&thread_249_3},
{&thread_250_3},
{&thread_251_3},
{&thread_252_3},
{&thread_253_3},
{&thread_254_3},
{&thread_255_3}
};

/* Define the test array.  This used to store the randomized test.  */

static TX_THREAD               *_smp_randomized_test_array[TX_THREAD_SMP_MAX_CORES*2];


/* Define thread entry prototype. Since it won't be used it can be the same.  */

static void    control_thread_entry(ULONG thread_input);
static void    thread_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);



static UINT    pass;
static UINT    start_pass;
static UINT    end_pass;
static UINT    mapping_error;


/* Create a test control thread.  */

static TX_THREAD    control_thread;


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_smp_random_resume_suspend_exclusion_test(void *first_unused_memory)
#endif
{

CHAR        *pointer;
UINT        status;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Create a control thread to run the test.  */
    status =  tx_thread_create(&control_thread, "control thread", control_thread_entry, 0,  
            pointer, 1024, 
            0, 0, TX_NO_TIME_SLICE, TX_AUTO_START);     

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Random Suspensions/Resumptions/Exclusions Test.......... ERROR #1\n");
        test_control_return(1);
    }
}


static void    control_thread_entry(ULONG thread_input)
{

UINT        i, j;
UINT        priority;
UINT        source_index;
UINT        successful_tests =  0;
UINT        test_errors =  0;
TX_THREAD   *thread_ptr;
TX_THREAD   *current_thread;
UINT        original_priority;
ULONG       exclusions;
UINT        status;


    /* Clear mapping error flag.  */
    mapping_error =  TX_FALSE;

    /* Pickup the current thread pointer.  */
    current_thread =  tx_thread_identify();

    /* Loop to create all the threads.  */
    i =  0;
    priority =  0;
    status =  TX_SUCCESS;
    while (i < 1024)
    {

        /* Create each thread.  */
        status +=  tx_thread_create(_smp_randomized_source_array[i], "test thread", thread_entry, i,  
//            (void *) pointer, 512, 
            malloc(1024), 1024,
            priority, priority, TX_NO_TIME_SLICE, TX_DONT_START);     
//        pointer =  pointer + 512;

        /* Check status.  */
        if (status != TX_SUCCESS)
        {

            printf("Running SMP Random Suspensions/Resumptions/Exclusions Test.......... ERROR #2\n");
            test_control_return(1);
            break;
        }

        /* Move to next entry/priority.  */
        i++;
        priority++;
        
        /* Should priority be reset?  */
		if (priority >= TX_MAX_PRIORITIES)
		{

			/* Yes, reset the priority.  */
			priority =  0;
		}
    }

    /* Start random test.  */
    printf("Running SMP Random Suspensions/Resumptions/Exclusions Test.......... ");

    /* Clear system counters.  */
    pass =         0;
    start_pass =   0;
    end_pass =     start_pass + MAX_PASSES;
    do
    {

        /* Clear the randomized test array.  */
        for (i = 0; i < (TX_THREAD_SMP_MAX_CORES*2); i++)
        {
            _smp_randomized_test_array[i] = TX_NULL;
        }        
 
		/* Build the randomized test array.  */
        for (i = 0; i < (TX_THREAD_SMP_MAX_CORES*2); i++)
        {
            do
            {

                source_index =  (rand())%1024;
                
                /* Determine if this index has repeated.  */
                thread_ptr = _smp_randomized_source_array[source_index];

                /* Is this thread already in the test array?  */
                j =  0;
                while (j < TX_THREAD_SMP_MAX_CORES*2)
                {

                    /* Is the entry NULL?  */
                    if (_smp_randomized_test_array[j] == TX_NULL)
                    {
                        j =  (TX_THREAD_SMP_MAX_CORES*2);
                    }
                    
                    /* Determine if we have a duplicate.  */
                    if (_smp_randomized_test_array[j] == thread_ptr)
						thread_ptr =  TX_NULL;
                
                    j++;
                }
              
             } while (thread_ptr == TX_NULL);
            
            /* Clear run counter.  */
            thread_ptr -> tx_thread_run_count =  0;
            
            /* Setup the exclusion for this thread.  */
            exclusions =  (ULONG) (rand()%15);
            tx_thread_smp_core_exclude(thread_ptr, exclusions);

            /* Save the thread pointer.  */
            _smp_randomized_test_array[i] =  thread_ptr;
        }
    
        /* Now make all the random threads ready.  */
        for (i = 0; i < (TX_THREAD_SMP_MAX_CORES*2); i++)
        {
            status =  tx_thread_resume(_smp_randomized_test_array[i]);

            /* Check for an error.  */  
            if (status != TX_SUCCESS)
            {
                          
                printf("ERROR #3\n");
                test_control_return(1);
				break;
            } 
        }
        
        /* Check the status.  */
        if (status)
            break;
        
        /* Move to the lowest priority.  */
        status    +=  tx_thread_priority_change(current_thread, TX_MAX_PRIORITIES-1, &original_priority);
		tx_thread_relinquish();
            
        /* At this point all the threads have run, or should have.  */
        
        /* Restore priority.  */
        status   +=  tx_thread_priority_change(current_thread, original_priority, &original_priority);
             
        /* Was there an error?  */
        if (status != TX_SUCCESS)
        {
                          
             printf("ERROR #4\n");
             test_control_return(1);
             break;
        } 
                   
        /* Determine if all the threads in the the random sample ran.  */
        for (i = 0; i < (TX_THREAD_SMP_MAX_CORES*2); i++)
        {

            /* Pickup the thread pointer.  */
			thread_ptr =  _smp_randomized_test_array[i];

            /* Check to see if each thread has run.  */
            if (thread_ptr -> tx_thread_run_count == 0)
            {

                /* First, try to sleep to see if this helps!  */
                tx_thread_sleep(1);
            }

			/* Has the run count incremented?  */
            if (thread_ptr -> tx_thread_run_count == 0)
            {

				 /* No, this thread didn't really run!  */
                 printf("ERROR #5\n");
                 test_control_return(1);
                 break;
            }

			/* Make sure this thread suspended.  */
            while (thread_ptr -> tx_thread_state != TX_SUSPENDED)
            {

				/* Wait for the thread to complete!  */
				tx_thread_relinquish();
			}
            
            /* Reset the exclusion for this thread.  */
            tx_thread_smp_core_exclude(thread_ptr, 0);
            
            /* Check for mapping error.  */
            if (mapping_error)
            {
            
				 /* No, this thread ran on inccorect core!  */
                 printf("ERROR #6\n");
                 test_control_return(1);
                 break;
            }
        }         
                  
        /* Increment the pass counter.  */
        pass++;

    } while (pass < end_pass);

    /* Test is successful!  */
    printf("SUCCESS!\n");
    test_control_return(0);
}


static void thread_entry(ULONG id)
{

UINT        core;
ULONG       core_bit_map;
TX_THREAD   *thread_ptr;

    /* While forever loop!  */
    while(1)
    {

        /* Get thread.  */
        thread_ptr =  _smp_randomized_source_array[id];
        
        /* Determine if this thread is running on a valid core.  */
        core =  tx_thread_smp_core_get();
        
        /* Build a bit map for this core.  */
        core_bit_map =  (((ULONG) 1) << core);
        
        /* Is this a valid core?  */
        if (core_bit_map & thread_ptr -> tx_thread_smp_cores_excluded)
        {
        
            /* Invalid core, set error flag.  */
            mapping_error =  TX_TRUE;
        }

        /* Suspend thread!  */
        tx_thread_suspend(thread_ptr);
    }
}

