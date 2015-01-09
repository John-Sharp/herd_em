convert 0.png -resize 12.5% N_still.png
convert 1.png 2.png 3.png 2.png -background 'rgba(0,0,0,0)' -resize 12.5% +append N_walking.png

convert 0.png -background 'rgba(0,0,0,0)' -rotate 45 -resize 12.5% NE_still.png
convert 1.png 2.png 3.png 2.png -background 'rgba(0,0,0,0)' -rotate 45 -resize 12.5% +append NE_walking.png

convert 0.png -background 'rgba(0,0,0,0)' -rotate 90 -resize 12.5% E_still.png
convert 1.png 2.png 3.png 2.png -background 'rgba(0,0,0,0)' -rotate 90 -resize 12.5% +append E_walking.png

convert 0.png -background 'rgba(0,0,0,0)' -rotate 135 -resize 12.5% SE_still.png
convert 1.png 2.png 3.png 2.png -background 'rgba(0,0,0,0)' -rotate 135 -resize 12.5% +append SE_walking.png

convert 0.png -background 'rgba(0,0,0,0)' -rotate 180 -resize 12.5% S_still.png
convert 1.png 2.png 3.png 2.png -background 'rgba(0,0,0,0)' -rotate 180 -resize 12.5% +append S_walking.png

convert 0.png -background 'rgba(0,0,0,0)' -rotate 225 -resize 12.5% SW_still.png
convert 1.png 2.png 3.png 2.png -background 'rgba(0,0,0,0)' -rotate 225 -resize 12.5% +append SW_walking.png

convert 0.png -background 'rgba(0,0,0,0)' -rotate 270 -resize 12.5% W_still.png
convert 1.png 2.png 3.png 2.png -background 'rgba(0,0,0,0)' -rotate 270 -resize 12.5% +append W_walking.png

convert 0.png -background 'rgba(0,0,0,0)' -rotate 315 -resize 12.5% NW_still.png
convert 1.png 2.png 3.png 2.png -background 'rgba(0,0,0,0)' -rotate 315 -resize 12.5% +append NW_walking.png
