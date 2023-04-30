Программа для перемещения файлов в корзину, вместо их безвозвратного удаления

Использование:

make

LD_PRELOAD=./build/unlink.so rm <filepath>        // для вызова unlinkat(2)

LD_PRELOAD=./build/unlink.so unlink <filepath>    // для вызова unlink(2)

Также можно с помощью одной команды удалять сразу несколько файлов (работает только для rm,
так как unlink поддерживает удаление только одного файла):

LD_PRELOAD=./build/unlink.so rm <filepath1> <filepath2> ...

