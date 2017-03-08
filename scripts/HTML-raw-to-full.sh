cd ../Raw_HTML/

arr=( * )

len=${#arr[@]}
for (( i=0; i<$len; i++))
do
	echo "$i: ${arr[$i]}"
	if [ $i == 0 ]
	then
		arrows="<div class=\"next\"><a href=\"${arr[1]}\"><i class=\"material-icons\">arrow_forward</i></a></div>"
	elif [ $i == $((len - 1)) ]
	then
		arrows="<div class=\"back\"><a href=\"${arr[(($i-1))]}\"><i class=\"material-icons\">arrow_back</i></a></div>"
	else
		arrows="<div class=\"back\"><a href=\"${arr[(($i-1))]}\"><i class=\"material-icons\">arrow_back</i></a></div>\
        <div class=\"next\"><a href=\"${arr[(($i+1))]}\"><i class=\"material-icons\">arrow_forward</i></a></div>"

	fi
	f=${arr[$i]}
	echo "$(cat ../scripts/header.html)\n$(cat "$f")\n$arrows\n$(cat ../scripts/footer.html)" > "../Full HTML/$f"
done
