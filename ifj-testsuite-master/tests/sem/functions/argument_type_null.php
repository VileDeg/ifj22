//compiler 0
//
<?php
declare(strict_types=1);

function fun (?string $p1, ?int $p2, ?float $p3) : void
{}

fun(null, null, null);

return 0;