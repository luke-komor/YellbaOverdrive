<Qucs Schematic 0.0.19>
<Properties>
  <View=-44,-210,1052,1294,1.21,0,0>
  <Grid=5,5,1>
  <DataSet=in_buff.dat>
  <DataDisplay=in_buff.dpl>
  <OpenDisplay=0>
  <Script=in_buff.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Rectangle -55 -45 110 136 #000000 5 1 #c0c0c0 1 0>
  <.ID -55 -86 SUB>
  <Text -40 20 10 #000000 0 "BIAS">
  <Text -45 -40 10 #000000 0 "GND">
  <Text -40 -10 10 #000000 0 "VCC\n">
  <Text -43 61 10 #000000 0 "IN">
  <.PortSym -65 30 4 0>
  <.PortSym -65 0 3 0>
  <.PortSym -65 -30 5 0>
  <.PortSym -65 70 1 0>
  <Line -65 -30 10 0 #000000 5 1>
  <Line -65 0 10 0 #000000 5 1>
  <Line -65 30 10 0 #000000 5 1>
  <Line -65 70 10 0 #000000 5 1>
  <.PortSym 70 70 2 180>
  <Text 10 60 10 #000000 0 "OUT">
  <Line 70 70 -15 0 #000000 5 1>
</Symbol>
<Components>
  <C C2 1 380 320 -26 17 0 0 "1 u" 1 "" 0 "neutral" 0>
  <C C3 1 320 400 -26 17 0 0 "1u" 1 "" 0 "neutral" 0>
  <R R1 1 280 320 -26 15 0 0 "100k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R2 1 200 400 -26 15 0 0 "1k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <R R3 1 390 480 15 -26 0 1 "510k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <C C1 1 670 360 -26 17 0 0 "1 u" 1 "" 0 "neutral" 0>
  <Port vcc 1 520 231 -65 -23 0 3 "3" 1 "analog" 0>
  <GND * 1 583 237 0 0 0 0>
  <Port gnd 1 573 211 -23 12 0 0 "5" 0 "analog" 0>
  <Port out 1 760 360 4 -33 0 2 "2" 0 "analog" 0>
  <R R4 1 730 410 15 -26 0 1 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Port bias 1 390 560 12 4 0 1 "4" 0 "analog" 0>
  <Port in 1 130 400 -23 12 0 0 "1" 0 "analog" 0>
  <OpAmp OP1 1 500 360 -26 -63 1 0 "300000" 0 "6" 0>
</Components>
<Wires>
  <310 320 350 320 "" 0 0 0 "">
  <350 400 390 400 "" 0 0 0 "">
  <390 400 390 450 "" 0 0 0 "">
  <230 400 290 400 "" 0 0 0 "">
  <130 400 170 400 "in" 135 370 10 "">
  <200 320 250 320 "bias" 209 287 11 "">
  <390 510 390 560 "bias" 421 526 27 "">
  <700 360 730 360 "out" 680 325 8 "">
  <583 211 583 237 "" 0 0 0 "">
  <573 211 583 211 "" 0 0 0 "">
  <410 285 410 320 "" 0 0 0 "">
  <410 285 585 285 "" 0 0 0 "">
  <585 360 640 360 "" 0 0 0 "">
  <585 285 585 360 "" 0 0 0 "">
  <460 380 460 400 "" 0 0 0 "">
  <390 400 460 400 "" 0 0 0 "">
  <460 320 460 340 "" 0 0 0 "">
  <410 320 460 320 "" 0 0 0 "">
  <460 340 470 340 "" 0 0 0 "">
  <460 380 470 380 "" 0 0 0 "">
  <540 360 585 360 "" 0 0 0 "">
  <730 360 760 360 "" 0 0 0 "">
  <730 360 730 380 "" 0 0 0 "">
  <730 440 730 460 "bias" 760 440 11 "">
  <390 400 390 400 "in_pre" 405 370 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
