
0x0000 +----------------------
       | Zero page
0x0100 +----------------------
       | Stack 
0x0300 +----------------------
       |
  ...  | Free
       |
0x7C40 +----------------------
       | VRAM (960B)*
0x8000 +----------------------
       |
  ...  | ROM
       |
0xFFFA | NMIB (LO) -- Non-maskable interrupt
0xFFFB | NMIB (HI)
0xFFFC | RESB (LO) -- Reset vector
0xFFFD | RESB (HI)
0xFFFE | BRK/IRQ (LO) -- Break / Interrupt Req
0xFFFF | BRK/IRQ (HI)
       +----------------------

VRAM size
---------
screen = 640x480 pixels 
1 char = 20x16
=> 40x24 chars = 960 chars = 960B needed for VRAM