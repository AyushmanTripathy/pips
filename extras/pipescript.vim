set filetype=pipescript

syntax match keywords /fn\|def\|if\|elif\|else/
syntax match functions /return\|pass\|self/
syntax match strings /"[^"]*"/
syntax match values /[0-9]\|Null\|False\|True/
syntax match comments /[/#].*/
syntax match pipe /|\|\[\|\]\|:/

highlight keywords ctermfg=167
highlight functions ctermfg=214
highlight strings ctermfg=142
highlight values ctermfg=175
highlight comments ctermfg=244
highlight pipe ctermfg=244
