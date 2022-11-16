//compiler 0
//
<?php
declare(strict_types=1);

function funA () : void
{}
function funB (int $a, string $b) : void
{}

funA();
funB(5, "help");

return 0;