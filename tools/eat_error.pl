#!/usr/bin/perl
use strict;
use warnings;

$^I = ".bak";

while (<>) {
    print if (/^PT : \d+$/ or /^NT : \d+$/ or /^PE : \d+ \d+$/ or /^NE : \d+ \d+$/);
}
