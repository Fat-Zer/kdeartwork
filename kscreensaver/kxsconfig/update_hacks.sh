#!/bin/sh
scsvs=`cd /usr/lib/xscreensaver/ && ls -1 | grep -v config`
echo $scsvs
for name in $scsvs; do
    if test -f /usr/lib/xscreensaver/$name && test -f /usr/share/xscreensaver/config/$name.xml && test ! -f ScreenSavers/$name.desktop; then
        echo "name: $name"
        label=`grep '<screensaver.*_label' /usr/share/xscreensaver/config/$name.xml | sed -e 's#^.*_label=\"\(.*\)\".*$#\1#'`
        echo "label: $label"
        f=ScreenSavers/$name.desktop
        sed -e "s#@NAME@#$name#; s#@LABEL@#$label#" ScreenSavers/xscreensaver.template > $f
        if ldd /usr/lib/xscreensaver/$name 2>&1 | grep libGL; then
            sed -e "s,@GL1@,X-KDE-Category=OpenGL Screen Savers,; s,@GL2@,X-KDE-Type=OpenGL," $f > $f.new
        else
            grep -v @GL $f > $f.new
        fi
        mv $f.new $f
    fi
done 
