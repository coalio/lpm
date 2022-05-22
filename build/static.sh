cd ../src/static/assets

echo "
#pragma once

// messages.h contains strings that I can't be bothered to write inside the
// source files.

" > ../messages.h
xxd -i MESSAGES_HELP | sed 's/\([0-9a-f]\)$/\0, 0x00/' >> ../messages.h


echo "
#pragma once

// defaults.h contains default file contents such as the default config toml file.

" > ../defaults.h
xxd -i LPM_TOML_DEFAULT | sed 's/\([0-9a-f]\)$/\0, 0x00/' >> ../defaults.h
xxd -i LPM_SQL_INIT_MIGRATION | sed 's/\([0-9a-f]\)$/\0, 0x00/' >> ../defaults.h