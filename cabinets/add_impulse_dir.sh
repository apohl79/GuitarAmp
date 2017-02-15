#!/bin/bash
if [ -z "$1" ]; then
    echo "Usage: $0 <dir>"
    exit 1
fi

for f in $1/*; do
    h="$(basename $f|sed 's/.wav$//').h"
    ./ir_wav2h.py -i $f -o $h
done

echo "Updating all.h ..."
echo "#ifndef CABINETS_ALL_H"  > all.h
echo "#define CABINETS_ALL_H" >> all.h
echo "" >> all.h
echo "int add_ir(const char* name, const char* vendor, const char* mic, std::vector<double>& frames, int rate) {" >> all.h
echo "    gImpulses.push_back(Impulse(name, vendor, mic, frames, rate));" >> all.h
echo "    return 0;" >> all.h
echo "}" >> all.h
echo "" >> all.h
for h in *.h; do
    if [ "$h" != "all.h" ]; then
        echo "#include \"$h\"" >> all.h
    fi
done
echo "#endif" >> all.h
