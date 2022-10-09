<?php
/** 
 * ifj22.php - Modul pro zlepseni kompatibility PHP 8 a IFJ22
 * 
 * Skript pomaha preklenout zakladni rozdily v obou jazycich pro ucely testovani, 
 * avsak zdaleka neodstranuje vsechny odlisnosti! 
 * Pouziti (predejte tomuto skriptu nazev vaseho souboru jako jediny parametr (na serveru Merlin): 
 * 		php8.1 ifj22.php vas_skript.php
 * 
 * PHP version 8
 * 
 * @author Zbyněk Křivka <krivka@fit.vut.cz>
 * @author Radim Kocman <kocman@fit.vut.cz>
 * 
 */

declare(strict_types=1);

error_reporting(E_ALL);

if (!defined("STDIN"))
	define("STDIN", fopen('php://stdin', 'r'));

function reads(): ?string
{
	$line = fgets(STDIN);
	if ($line === false) //EOF
		return null;
	return preg_replace('~(\r)?(\n)?$~', "", $line);
}

function readi(): ?int
{
	$line = reads();
	if ($line === null) //EOF
		return null;
	return filter_var($line, FILTER_VALIDATE_INT, FILTER_NULL_ON_FAILURE | FILTER_FLAG_ALLOW_OCTAL | FILTER_FLAG_ALLOW_HEX);
} 

function readf(): ?float
{
	$line = reads();
	if ($line === null) //EOF
		return null;
	return filter_var($line, FILTER_VALIDATE_FLOAT, FILTER_NULL_ON_FAILURE);
}

function write(): void
{
	foreach(func_get_args() as $arg)
	{
		if (is_double($arg))
			printf("%g", $arg);
		else
			print strval($arg);
	}
	return;
}

function substring(string $s, int $i, int $j): ?string
{
	$s = strval($s);
	$i = intval($i);
	$j = intval($j);
	if ($i < 0
		|| $j < 0
		|| $i > $j
		|| $i >= strlen($s)
		|| $j > strlen($s))
		return null;
	return substr($s, $i, $j - $i);	
}

if (isset($argv[1]))
{
	if (!file_exists($argv[1]))
		die("File $argv[1] does not exist!");
	return include($argv[1]);
}

?>