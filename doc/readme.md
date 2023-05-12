Программы unlink.so и trash для перемещения файлов в корзину, вместо их безвозвратного удаления:

Использование unlink.so:

make

LD_PRELOAD=./build/unlink.so rm <filepath>        // для вызова unlinkat(2)

LD_PRELOAD=./build/unlink.so unlink <filepath>    // для вызова unlink(2)

Также можно с помощью одной команды удалять сразу несколько файлов (работает только для rm,
так как unlink поддерживает удаление только одного файла):

LD_PRELOAD=./build/unlink.so rm <filepath1> <filepath2> ...

Использование trash:

make

./build/trash

Далее выводится следуещее меню, где можно выбирать различные опции:

------------------------------------------------
menu:
q - exit
l - list trash files
p - put file into trash
r - restore file from trash
d - delete file from trash permanently
c - clear trashbin
m - print menu
------------------------------------------------
