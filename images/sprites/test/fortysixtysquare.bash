convert -size 100x100 canvas:blue bg.png
convert -size 40x60 canvas:green fg.png
convert bg.png fg.png -alpha on -compose dissolve -define compose:args=100,20 -gravity center -composite fortysixtysquare.png
rm bg.png
rm fg.png
