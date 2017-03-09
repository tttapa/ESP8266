from=(	"716" 
	"731" 
	"746"
	"743"
	"747"
	"744"
	"748"
	"738"
	"764"
	"798"
	"802"
	"803"
	"805"
	"806"
	"808"
	"811"
	"812")
to=(	"VReg.png" 
	"ESP8266-2.png"
	"reset_without_diode.png"
	"without_diode.png"
	"reset_with_diode.png"
	"with_diode.png"
	"auto-reset_auto-program.png"
	"station.png"
	"http_chrome.png"
	"chrome_post_LED.png"
	"ESP_OTA.png"
	"RGB_rainbow.png"
	"LED_Control.png"
	"Edit.png"
	"ESP8266_Temperature_Logger.png"
	"blocked sign-in attempt-2.png"
	"less secure apps-2.png")

cp -a "../Raw_HTML_Local/." "../Raw_HTML/"

cd ../Raw_HTML/

len=${#from[@]}
for (( i=0; i<$len; i++))
do
	replace "/photo.php?id=${from[$i]}&amp;thumb=1" "../Images/thumbs/${to[$i]}" "/photo.php?id=${from[$i]}" "../Images/${to[$i]}" -- *
done
