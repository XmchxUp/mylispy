echo "=====================cloc====================="
cloc --not-match-f 'mpc.[ch]' ./src
echo "=====================find====================="
find . -name '*.[ch]' ! -name 'mpc.[ch]' | \
    xargs wc -l | \
    sort