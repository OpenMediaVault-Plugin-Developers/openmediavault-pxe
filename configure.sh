#!/bin/bash

echo "Enter plugin details: "
read -p "Name: " name

name=$(echo ${name} | sed 's/^ *//g' | sed 's/ *$//g')

if [ -z "${name}" ]; then
    echo "Empty name entered, exiting ..."
    exit 1
fi

name_lowercase=${name,,}
name_lowercase=$(echo ${name_lowercase} | tr -d ' ')

name_camelcase=$(echo ${name} | sed 's/[^ ]\+/\u&/g')
name_camelcase=$(echo ${name_camelcase} | tr -d ' ')

echo "Updating configuration ..."

# debian/*
sed -i -e "s/skeleton/${name_lowercase}/g" debian/*
sed -i -e "s/Skeleton/${name}/g" debian/*

# usr/share/openmediavault/*
find usr/share/openmediavault/ -type f -exec sed -i -e "s/skeleton/${name_lowercase}/g" {} \;
find usr/share/openmediavault/ -type f -exec sed -i -e "s/Skeleton/${name_camelcase}/g" {} \;

find usr/share/openmediavault/ -type f \
                               -name skeleton* \
                               -execdir rename "s/skeleton/${name_lowercase}/" {} \;
find usr/share/openmediavault/ -type f \
                               -name Skeleton* \
                               -execdir rename "s/Skeleton/${name_camelcase}/" {} \;

# var/www/openmediavault/*
find var/www/openmediavault/ -type f -exec sed -i -e "s/skeleton/${name_lowercase}/g" {} \;
find var/www/openmediavault/ -type f -exec sed -i -e "s/Skeleton/${name_camelcase}/g" {} \;

find var/www/openmediavault/ -type f \
                             -name skeleton* \
                             -execdir rename "s/skeleton/${name_lowercase}/" {} \;
find var/www/openmediavault/ -type f \
                             -name Skeleton* \
                             -execdir rename "s/Skeleton/${name_camelcase}/" {} \;
rename "s/skeleton/${name_lowercase}/" var/www/openmediavault/js/omv/module/admin/service/skeleton

rm -rf .git
rm README.md
rm configure.sh
rename "s/skeleton/${name_lowercase}/" ../openmediavault-skeleton

echo "Done!"

exit 0
