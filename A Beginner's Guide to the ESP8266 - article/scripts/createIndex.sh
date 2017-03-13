titles=("Introduction" "Hardware" "Software" "Microcontroller" "Network protocols" "Uploading" "Wi-Fi connections" "mDNS" "Web servers" "Simple web server" "SPIFFS" "Uploading to the server" "OTA" "WebSocket" "NTP" "Data logging" "Email notifier" "Advanced" "In conclusion")

cd ../Raw_HTML/

if [ ! -e ../Indices/ ]
then
    mkdir ../Indices/
fi

arr=( * )

len=${#arr[@]}
for (( i=0; i<$len; i++))
do
	echo "$i: ${arr[$i]}"
	if [ $i == 0 ]
	then
		links="            <div class=\"next\"><a href=\"${arr[(($i+1))]}\">Next chapter →</a></div>"
	elif [ $i == $((len - 1)) ]
	then
		links="            <div class=\"back\"><a href=\"${arr[(($i-1))]}\">← Previous chapter</a></div>"
	else
		links="            <div class=\"back\"><a href=\"${arr[(($i-1))]}\">← Previous chapter</a></div>
            <div class=\"next\"><a href=\"${arr[(($i+1))]}\">Next chapter →</a></div>"
	fi

	if [ $i == 0 ]
	then
		arrows="            <div class=\"nextArr\"><a href=\"${arr[1]}\"><i class=\"material-icons\">arrow_forward</i></a></div>"
	elif [ $i == $((len - 1)) ]
	then
		arrows="            <div class=\"backArr\"><a href=\"${arr[(($i-1))]}\"><i class=\"material-icons\">arrow_back</i></a></div>"
	else
		arrows="            <div class=\"backArr\"><a href=\"${arr[(($i-1))]}\"><i class=\"material-icons\">arrow_back</i></a></div>
            <div class=\"nextArr\"><a href=\"${arr[(($i+1))]}\"><i class=\"material-icons\">arrow_forward</i></a></div>"
	fi

    nav=""
    for (( j=0; j<$len; j++))
    do
        l=${arr[$j]}
        if [ $j == $i ]
        then
            nav="$nav<li><b>${titles[$j]}</b></li>\r\n"
        else
            nav="$nav<li><a href=\"$l\">${titles[$j]}</a></li>\r\n"
        fi
    done

	f=${arr[$i]}
    echo "$(cat ../scripts/navHeader.html)" > "../Indices/$f"
    echo -e "$nav" >> "../Indices/$f"
    echo "$(cat ../scripts/navFooter.html)" >> "../Indices/$f"
	echo "$(cat "$f")" >> "../Indices/$f"
	echo "<hr>" >> "../Indices/$f"
	echo "$links" >> "../Indices/$f"
	echo "$arrows" >> "../Indices/$f"
	echo "$(cat ../scripts/footer.html)" >> "../Indices/$f"
done