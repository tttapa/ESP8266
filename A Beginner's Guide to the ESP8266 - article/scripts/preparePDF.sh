cd ../Raw_HTML/

if [ ! -e ../PDF_HTML/ ]
then
    mkdir ../PDF_HTML/
fi

arr=( * )

len=${#arr[@]}
for (( i=0; i<$len; i++))
do
	echo "$i: ${arr[$i]}"

	f=${arr[$i]}
	echo "$(cat ../scripts/PDFheader.html)" > "../PDF_HTML/$f"
	echo "$(cat "$f")" >> "../PDF_HTML/$f"
	echo "$(cat ../scripts/footer.html)" >> "../PDF_HTML/$f"
    sed -i "s#../Images/thumbs#../Images#g" "../PDF_HTML/$f"
done