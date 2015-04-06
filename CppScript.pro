TEMPLATE = subdirs

SUBDIRS += src
!CONFIG(NoTest): SUBDIRS += test

test.depends = src
