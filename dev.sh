
dev(){
  nodemon -w main.c -w test.pipescript -x "gcc main.c && ./a.out test.pipescript"
}
dev_tmp(){
  nodemon -w tmp.c -w test.pipescript -x "gcc tmp.c && ./a.out test.pipescript"
}

while getopts "m t" flag
do
    case "${flag}" in
        m) dev;;
        t) dev_tmp;;
    esac
done
