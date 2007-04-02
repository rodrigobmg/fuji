#if 0
//
// Generated by Microsoft (R) D3DX9 Shader Compiler 9.11.519.0000
//
//   fxc /FhMatStandard_a.h MatStandard.vsh /Tvs_1_1 /Zi /nologo /D_ANIMATION
//
//
// Parameters:
//
//   float4 bAnimation[2];
//   float4 gColourMask;
//   float4 gModelColour;
//   float4 mAnimMats[210];
//   float4 mTexMatrix[2];
//   float4x4 mWorldToScreen;
//
//
// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   mWorldToScreen c0       4
//   mTexMatrix     c8       2
//   gModelColour   c10      1
//   gColourMask    c11      1
//   bAnimation     c12      2
//   mAnimMats      c20    210
//

    vs_1_1
    def c4, 255, 0.5, 0, 0
    dcl_position v0  // input<0,1,2,3>
    dcl_texcoord v1  // input<7,8,9,10>
    dcl_color v2  // input<11,12,13,14>
    dcl_blendweight v3  // input<15,16,17,18>
    dcl_blendindices v4  // input<19,20,21,22>

#line 59 "MatStandard.vsh"
    mad r0, v4, c4.x, c4.y  // input<19,20,21,22>

#line 66
    mov a0.x, r0.y
    dp4 r1.x, v0, c20[a0.x]  // t1<0>
    dp4 r1.y, v0, c21[a0.x]  // t1<1>
    dp4 r1.z, v0, c22[a0.x]  // t1<2>

#line 78
    mul r2.xyz, r1, v3.y

#line 62
    mov a0.x, r0.x
    dp4 r1.x, v0, c20[a0.x]  // t0<0>
    dp4 r1.y, v0, c21[a0.x]  // t0<1>
    dp4 r1.z, v0, c22[a0.x]  // t0<2>

#line 78
    mad r1.xyz, r1, v3.x, r2

#line 70
    mov a0.x, r0.z
    dp4 r0.x, v0, c20[a0.x]  // t2<0>
    dp4 r0.y, v0, c21[a0.x]  // t2<1>
    dp4 r0.z, v0, c22[a0.x]  // t2<2>

#line 78
    mad r1.xyz, r0, v3.z, r1

#line 74
    mov a0.x, r0.w
    dp4 r0.x, v0, c20[a0.x]  // t3<0>
    dp4 r0.y, v0, c21[a0.x]  // t3<1>
    dp4 r0.z, v0, c22[a0.x]  // t3<2>
    mad r1.xyz, r0, v3.w, r1

#line 40
    mul r0, v0, c12  // pos<0,1,2,3>

#line 78
    mad r0.xyz, r1, c13, r0  // pos<0,1,2>

#line 86
    dp4 oPos.x, r0, c0  // output<0>
    dp4 oPos.y, r0, c1  // output<1>
    dp4 oPos.z, r0, c2  // output<2>
    dp4 oPos.w, r0, c3  // output<3>

#line 94
    mul r0, v2, c10

#line 89
    dp4 r1.x, v1, c8  // output<8>
    dp4 r1.y, v1, c9  // output<9>

#line 94
    mad oD0, r0, c11.xxxz, c11.yyyw  // output<4,5,6,7>

#line 89
    mov oT0, r1.xyyy  // ::main<8,9,10,11>

// approximately 43 instruction slots used
#endif

const DWORD g_vs11_main_a[] =
{
    0xfffe0101, 0x0135fffe, 0x47554244, 0x00000028, 0x0000049c, 0x00000000, 
    0x00000001, 0x00000038, 0x0000002d, 0x0000003c, 0x00000009, 0x000003e8, 
    0x000001a4, 0x5374614d, 0x646e6174, 0x2e647261, 0x00687376, 0x00000028, 
    0xffff0000, 0x00000640, 0xffff0000, 0x00000658, 0xffff0000, 0x00000664, 
    0xffff0000, 0x00000670, 0xffff0000, 0x0000067c, 0xffff0000, 0x00000688, 
    0x0000003b, 0x00000694, 0x00000049, 0x000006a8, 0x00000049, 0x000006b4, 
    0x00000049, 0x000006c0, 0x00000049, 0x000006cc, 0x00000049, 0x000006dc, 
    0x00000049, 0x000006ec, 0x00000049, 0x000006fc, 0x00000042, 0x00000710, 
    0x00000042, 0x0000071c, 0x00000043, 0x0000072c, 0x00000044, 0x0000073c, 
    0x0000004e, 0x0000074c, 0x0000003e, 0x0000075c, 0x0000003e, 0x00000768, 
    0x0000003f, 0x00000778, 0x00000040, 0x00000788, 0x0000004e, 0x00000798, 
    0x00000046, 0x000007ac, 0x00000046, 0x000007b8, 0x00000047, 0x000007c8, 
    0x00000048, 0x000007d8, 0x0000004e, 0x000007e8, 0x0000004a, 0x000007fc, 
    0x0000004a, 0x00000808, 0x0000004b, 0x00000818, 0x0000004c, 0x00000828, 
    0x0000004e, 0x00000838, 0x00000028, 0x0000084c, 0x0000004e, 0x0000085c, 
    0x00000056, 0x00000870, 0x00000056, 0x00000880, 0x00000056, 0x00000890, 
    0x00000056, 0x000008a0, 0x0000005e, 0x000008b0, 0x00000059, 0x000008c0, 
    0x0000005a, 0x000008d0, 0x0000005e, 0x000008e0, 0x00000059, 0x000008f4, 
    0x6e69616d, 0x736f7000, 0xababab00, 0x00030001, 0x00040001, 0x00000001, 
    0x00000000, 0x6f6c6f63, 0x75007275, 0xabab0076, 0x000001a9, 0x000001b0, 
    0x000001c0, 0x000001b0, 0x000001c7, 0x000001b0, 0x00000005, 0x000c0001, 
    0x00030001, 0x000001cc, 0x0000002c, 0x00090008, 0x000b000a, 0xabab0069, 
    0x00020000, 0x00010001, 0x00000001, 0x00000000, 0x0000000d, 0x00000000, 
    0x00000000, 0x75706e69, 0x6f6e0074, 0xab006d72, 0x00030001, 0x00030001, 
    0x00000001, 0x00000000, 0x67696577, 0x00737468, 0x69646e69, 0x00736563, 
    0x000001a9, 0x000001b0, 0x00000226, 0x0000022c, 0x000001c7, 0x000001b0, 
    0x000001c0, 0x000001b0, 0x0000023c, 0x000001b0, 0x00000244, 0x000001b0, 
    0x00000005, 0x00170001, 0x00060001, 0x0000024c, 0x00000001, 0x00010000, 
    0x00030002, 0x00000002, 0x00080007, 0x000a0009, 0x00000003, 0x000c000b, 
    0x000e000d, 0x00000004, 0x0010000f, 0x00120011, 0x00000005, 0x00140013, 
    0x00160015, 0x00000006, 0x00140013, 0x00160015, 0x7074756f, 0xab007475, 
    0x00000024, 0xffff0000, 0xffffffff, 0x00000025, 0x0001ffff, 0xffffffff, 
    0x00000026, 0xffffffff, 0xffff0002, 0x00000027, 0xffffffff, 0x0003ffff, 
    0x00000029, 0xffff0008, 0xffffffff, 0x0000002a, 0x0009ffff, 0xffffffff, 
    0x0000002b, 0x00050004, 0x00070006, 0x00000022, 0x00010000, 0x00030002, 
    0x00000023, 0x00010000, 0xffff0002, 0xab003074, 0x00000014, 0xffff0000, 
    0xffffffff, 0x00000015, 0x0001ffff, 0xffffffff, 0x00000016, 0xffffffff, 
    0xffff0002, 0xab003174, 0x0000000f, 0xffff0000, 0xffffffff, 0x00000010, 
    0x0001ffff, 0xffffffff, 0x00000011, 0xffffffff, 0xffff0002, 0xab003274, 
    0x00000019, 0xffff0000, 0xffffffff, 0x0000001a, 0x0001ffff, 0xffffffff, 
    0x0000001b, 0xffffffff, 0xffff0002, 0xab003374, 0x0000001e, 0xffff0000, 
    0xffffffff, 0x0000001f, 0x0001ffff, 0xffffffff, 0x00000020, 0xffffffff, 
    0xffff0002, 0x00000000, 0x000001a4, 0x000001e4, 0x00000001, 0x000001f4, 
    0x000001a4, 0x00000200, 0x00000204, 0x00000001, 0x00000214, 0x000001a4, 
    0x00000220, 0x0000027c, 0x00000006, 0x0000028c, 0x000001a4, 0x000002d4, 
    0x000001e4, 0x00000007, 0x000002dc, 0x000001a4, 0x000001a9, 0x000001b0, 
    0x00000002, 0x00000330, 0x000001a4, 0x00000348, 0x0000022c, 0x00000003, 
    0x0000034c, 0x000001a4, 0x00000370, 0x0000022c, 0x00000003, 0x00000374, 
    0x000001a4, 0x00000398, 0x0000022c, 0x00000003, 0x0000039c, 0x000001a4, 
    0x000003c0, 0x0000022c, 0x00000003, 0x000003c4, 0x7263694d, 0x666f736f, 
    0x52282074, 0x33442029, 0x20395844, 0x64616853, 0x43207265, 0x69706d6f, 
    0x2072656c, 0x31312e39, 0x3931352e, 0x3030302e, 0xabab0030, 0x0058fffe, 
    0x42415443, 0x0000001c, 0x00000127, 0xfffe0101, 0x00000006, 0x0000001c, 
    0x00000101, 0x00000120, 0x00000094, 0x000c0002, 0x00020002, 0x000000a0, 
    0x00000000, 0x000000b0, 0x000b0002, 0x00020001, 0x000000bc, 0x00000000, 
    0x000000cc, 0x000a0002, 0x00020001, 0x000000bc, 0x00000000, 0x000000d9, 
    0x00140002, 0x000200d2, 0x000000e4, 0x00000000, 0x000000f4, 0x00080002, 
    0x00020002, 0x000000a0, 0x00000000, 0x000000ff, 0x00000002, 0x00020004, 
    0x00000110, 0x00000000, 0x696e4162, 0x6974616d, 0xab006e6f, 0x00030001, 
    0x00040001, 0x00000002, 0x00000000, 0x6c6f4367, 0x4d72756f, 0x006b7361, 
    0x00030001, 0x00040001, 0x00000001, 0x00000000, 0x646f4d67, 0x6f436c65, 
    0x72756f6c, 0x6e416d00, 0x614d6d69, 0xab007374, 0x00030001, 0x00040001, 
    0x000000d2, 0x00000000, 0x7865546d, 0x7274614d, 0x6d007869, 0x6c726f57, 
    0x536f5464, 0x65657263, 0xabab006e, 0x00030003, 0x00040004, 0x00000001, 
    0x00000000, 0x315f7376, 0x4d00315f, 0x6f726369, 0x74666f73, 0x29522820, 
    0x44334420, 0x53203958, 0x65646168, 0x6f432072, 0x6c69706d, 0x39207265, 
    0x2e31312e, 0x2e393135, 0x30303030, 0xababab00, 0x00000051, 0xa00f0004, 
    0x437f0000, 0x3f000000, 0x00000000, 0x00000000, 0x0000001f, 0x80000000, 
    0x900f0000, 0x0000001f, 0x80000005, 0x900f0001, 0x0000001f, 0x8000000a, 
    0x900f0002, 0x0000001f, 0x80000001, 0x900f0003, 0x0000001f, 0x80000002, 
    0x900f0004, 0x00000004, 0x800f0000, 0x90e40004, 0xa0000004, 0xa0550004, 
    0x00000013, 0x80030003, 0x80ee0000, 0x00000001, 0x800c0001, 0x80440003, 
    0x00000013, 0x80030001, 0x80e40000, 0x00000002, 0x800f0002, 0x80e40000, 
    0x81e40001, 0x0000000c, 0x800f0001, 0x81e40001, 0x80e40001, 0x0000000c, 
    0x800f0000, 0x80e40000, 0x81e40000, 0x00000004, 0x800f0000, 0x80e40000, 
    0x80e40001, 0x80e40002, 0x00000001, 0xb0010000, 0x80550000, 0x00000009, 
    0x80010001, 0x90e40000, 0xa0e42014, 0x00000009, 0x80020001, 0x90e40000, 
    0xa0e42015, 0x00000009, 0x80040001, 0x90e40000, 0xa0e42016, 0x00000005, 
    0x80070002, 0x80e40001, 0x90550003, 0x00000001, 0xb0010000, 0x80000000, 
    0x00000009, 0x80010001, 0x90e40000, 0xa0e42014, 0x00000009, 0x80020001, 
    0x90e40000, 0xa0e42015, 0x00000009, 0x80040001, 0x90e40000, 0xa0e42016, 
    0x00000004, 0x80070001, 0x80e40001, 0x90000003, 0x80e40002, 0x00000001, 
    0xb0010000, 0x80aa0000, 0x00000009, 0x80010000, 0x90e40000, 0xa0e42014, 
    0x00000009, 0x80020000, 0x90e40000, 0xa0e42015, 0x00000009, 0x80040000, 
    0x90e40000, 0xa0e42016, 0x00000004, 0x80070001, 0x80e40000, 0x90aa0003, 
    0x80e40001, 0x00000001, 0xb0010000, 0x80ff0000, 0x00000009, 0x80010000, 
    0x90e40000, 0xa0e42014, 0x00000009, 0x80020000, 0x90e40000, 0xa0e42015, 
    0x00000009, 0x80040000, 0x90e40000, 0xa0e42016, 0x00000004, 0x80070001, 
    0x80e40000, 0x90ff0003, 0x80e40001, 0x00000005, 0x800f0000, 0x90e40000, 
    0xa0e4000c, 0x00000004, 0x80070000, 0x80e40001, 0xa0e4000d, 0x80e40000, 
    0x00000009, 0xc0010000, 0x80e40000, 0xa0e40000, 0x00000009, 0xc0020000, 
    0x80e40000, 0xa0e40001, 0x00000009, 0xc0040000, 0x80e40000, 0xa0e40002, 
    0x00000009, 0xc0080000, 0x80e40000, 0xa0e40003, 0x00000005, 0x800f0000, 
    0x90e40002, 0xa0e4000a, 0x00000009, 0x80010001, 0x90e40001, 0xa0e40008, 
    0x00000009, 0x80020001, 0x90e40001, 0xa0e40009, 0x00000004, 0xd00f0000, 
    0x80e40000, 0xa080000b, 0xa0d5000b, 0x00000001, 0xe00f0000, 0x80540001, 
    0x0000ffff
};