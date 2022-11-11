//compiler 0
//
<?php
declare(strict_types=1);

function funA () : int
{
    return 0;
}
function funB () : ?string
{
    return null;
}
function funC (int $a, float $b) : string
{
    return "help";
}

$varA = funA();
$varB = funB();
$varC = funC(5, 5.4);

return 0;