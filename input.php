<?php
declare(strict_types=1);

$res = 1 > null;
write($res, "\n");
$res = 2 > 0.0;
write($res, "\n");
$res = 2 < 0;
write($res, "\n");