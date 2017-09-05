# TRECVID AVS dataset

Der TRECVID AVS Datensatz (IACC.3) beinhaltet 4,593 Videos. In der Datei: iacc.3.collection.xml ist jedes Video mit der ID, den Dateinamen, die Quelle und den Dateityp aufgelistet. Im ersten Schritt wurde der ursprüngliche Dateiname jedes Videos auf die jewilige VideoID umbenannt. Das hatte den Grund, dass die originalen Dateinamen sehr lang sind und mehr als 150 Zeichen umfassen (Windows maximale Pfadlänge 255 Zeichen). Im Weiteren können insgesamt 335,944 Mastershots aus den 4,593 Videos extrahiert werden. Für jedes Video gibt es eine CSV Datei, die die Shotgrenzen (Start und Endposition) der Mastershots in dem Video angeben. Für die Evaluierungen gibt es eine erweiterte Ground-Truth, die 8,083 Mastershots in 30 unterschiedliche Queryarten unterteilt.

## Struktur
- Der Ordner **/trecvid16_data/videos** beinhaltet die Videos, mit der eindeuitge ID als Dateinamen. (Zuordnung: iacc.3.collection.xml)
- Der Ordner **./trecvid16_data/mastershot-boundaries** beinhaltet die Grenzen (Start- und Endframe) für jedes Video, gespeichert Zeilenweise in einer CSV Datei.
- Der Ordner **./trecvid16_data/mastershots**
  - Der Ordner **./all** beinhaltet die Videoteile der einzelnen Videos. Der Dateiname besteht aus VideoID (iacc.3.collection.xml), ShotID (inkrementell lt. mastershot-boundaries, beginnend bei 1), Startframe, Endframe, FPS, Width, Height.
  - Der Ordner **./one-frame-mastershots** beinhaltet nur die ein Frame langen Mastershots.
  - Der Ordner **./one-second-mastershots** beinhaltet nur die ein Mastershots die kürzer als 1 Sekunde sind, berechnet in Abhängigkeit zu der FPS.
**./filtered** beinhaltet die restlichen Mastershots, die letztendlich für die Auswertung verwendet werden können.
  - Die Datei **all-mastershots.csv** listet alle Shots mit Dateinamen und Eigenschaften auf. Die letzte Spalte markiert ob der jeweilige Shot kürzer als 1 Sekunde ist.
  - Die Datei **extra.avs.qrels.tv16.xlsx** ist eine erwetierte Ground Truth zu dem AVS Datensatz die auf der Webiste, die auf TRECVID Website (vergangene Daten) bereitgestellt wird: [TRECVID Data Availability 2016](http://www-nlpir.nist.gov/projects/trecvid/trecvid.data.html#tv16) [*=> Extra Ad-hoc video search judgments made by VBS and Klagenfurt University*](http://www-nlpir.nist.gov/projects/tv2016/pastdata/extra.avs.qrels.tv16.xlsx).
  - Die Datei **./trecvid16_data/mastershots/rel-extra.avs.qrels.tv16** beinhaltet nur die relevant Mastershots, die letztendlich für eine Retireval Auswertung verwendet werden.
  - Die Datei **./trecvid16_data/mastershots/filt-rel-extra.avs.qrels.tv16** beinhaltet nur die relevant und gefilterten Mastershots. Dazu wurden die Mastershots, die kleiner als $MAX Sekunden haben, aus der rel-GT entfernt da diese auch nicht mehr in den Mastershots berücksichtig werden.


## Scripts and Progs
1. **trecvid-avs.filenameconverter** ist ein C++ Programm, welches die Dateienamen der Videos, und der Mastershot-Grenzen nach der jeweiligen VideoID umbenennt.
2. **divdeo.sh** ist ein Shell-Script, dass eine Videodatei und ihrer Mastershot-Grenzen verarbeitet und die einzelnen Videoteile als MP4 Dateien extrahiert. (Mastershots, die nur ein Frame lang sind, werden als .jpg erstellt)
3. **toosmall.sh** ist ein Shell-Script, das die Länge einer Videodatei analysiert und drei Listen erstellt (All, Waste, New). Die Liste "All" beinhaltet alle Mastershots und markiert die Shots in der letzten Spalte mit 1, die kürzer als *$MAX* Sekunde sind. "New" beinhaltet eine Liste mit Shots, die größer gleich *$MAX* Sekunde sind und "Waste" beinhaltet alle Mastershots, die nicht das Kriterium erfüllen.
4. **mergecsv.sh** ist ein Shell-Script, dass Einträge in CSV Dateien in einer Datei zusammenführt. Dabei wird die originale Datei als ID in die erste Spalte eingefügt.
5. **moshtoo.sh** ist ein Shell-Script, dass Videodateien mit einem bestimmten Kriterium in einen anderen Ordner kopiert. Als Kriterium reicht eine CSV Liste mit den zu kopierenden Mastershot Dateinamen.
8. **createIndices.sh** ist ein Shell-Script, dass zwei Index Dateien (trecvid16-video-list.csv, trecvid16-query-list.csv) für die Auswertungen generiert. Als erster Input dient ein Ordner, der alle auszuwertenden Dateien beinhaltet. Der zweite Input ist die Groundtruth als CSV Datai mit Matchingkriteria, VideoID und ShotID in jeder Zeile und mit "," getrennt.
9. **parallelz.sh** ist ein Shell-Script, dass es ermöglicht andere Shell-Scripts zu parallelisieren. Wenn jedoch vorhanden, kann dies auch über ***find | xargs -I{} bashscript.sh*** gelöst werden.

## Commands
```bash
#compile vretbox for linux and opencv installation in /opt
make opencv=opt os=linux

#Copy all files into another directory
find ./soourcedir/ -type f -name *.mp4* -print0 | xargs -0 -I{} ./destination/dir/
find ./soourcedir/ -type f -name "*.jpg*" -exec cp "{}" ./destination/dir/ \;

#Start a background process and redirect outputs to file
nohup bash bashscript.sh &> ../output-nohup.out&
#Kill process
kill PID

#Set environemt variable
export VRET_DATA="/path/to"
export VRET_EVAL="/path/to"

#Execute pre-build.sh to set env variable in current terminal use the point in front of
. pre-build.sh

#Read os errors
dmesg | tail
```

## Auswertungen
Für die Evaluierung wurden nur Mastershots verwendet, die mindestens eine Länge von einer Sekunde haben. Das sind insgesamt nur **186,630** Shots von den *335,944* Mastershots. Für die Auswertungen wurde auch die Ground-Truth aktualisiert. Es bleiben **6,649** Einträge von *8,083*, welche das Kriterium erfüllen.
