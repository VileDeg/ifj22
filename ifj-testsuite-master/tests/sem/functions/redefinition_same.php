//compiler 3
//
<?php
declare(strict_types=1);

function fun () : int
{
    return 0;
}

function fun() : int
{
    return 1;
}

fun();

return 0;