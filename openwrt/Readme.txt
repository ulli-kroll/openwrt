GPL code for  WR11000/M11000/AP11000

Build WR11000 Guide:
1. cd ./21.02
2. make R65_oemconfig
3. make V=s -j4

Build M11000 Guide:
1. cd ./21.02
2. make R86_oemconfig
3. make V=s -j4

Build AP11000 Guide:
1. cd ./21.02
2. make R87_oemconfig
3. make V=s -j4


Notice:
1. When you are tring to build GPL code, the make program will automatic download
   some other source code packages that it needs from Internet, please make sure 
   your Linux PC have good Internet connection.

2. If you are of the opinion that CUDY should offer further source code subject
   to the GPL, please contact us under 'support@cudy.com'.
