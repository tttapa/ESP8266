echo "Creating HTML files to convert to PDF ..."
./preparePDF.sh
echo -e "Done \r\n\r\nConverting to PDF..."

if [ ! -e ~/bin/wkhtmltopdf/ ]
then
    wget -P /tmp/ "http://download.gna.org/wkhtmltopdf/0.12/0.12.4/wkhtmltox-0.12.4_linux-generic-amd64.tar.xz"
    mkdir ~/bin/wkhtmltopdf/ && tar xfJ wkhtmltox-0.12.4_linux-generic-amd64.tar.xz -C ~/bin/wkhtmltopdf/ --strip-components 1
fi

dpkg -l | grep ttf-mscorefonts-installer > /dev/null
if [ $? != "0" ]
then
    wget -P /tmp/ http://ftp.de.debian.org/debian/pool/contrib/m/msttcorefonts/ttf-mscorefonts-installer_3.6_all.deb
    sudo dpkg -i ttf-mscorefonts-installer_3.6_all.deb
    sudo apt-get install -f
fi

PATH=$PATH:/home/$USER/bin/wkhtmltopdf/bin

cd ../PDF_HTML/

if [ ! -e ../PDF/ ]
then
    mkdir ../PDF/
fi

arr=( * )

len=${#arr[@]}
for (( i=0; i<$len; i++))
do
	echo "$i: ${arr[$i]}"

	f=${arr[$i]}
	wkhtmltopdf "$f" "../PDF/$(($i+1)).pdf"
done

cd ../PDF/

arr=( $(ls -I "A-Beginner's-Guide-to-the-ESP8266.pdf") )
sorted=($(for l in ${arr[@]}; do echo $l; done | sort -g ))

echo -e "\r\nCombining chapters ..."
pdftk ${sorted[@]} cat output "A-Beginner's-Guide-to-the-ESP8266.pdf"
echo "Done"