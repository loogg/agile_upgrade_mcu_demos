# RT-Thread building script for bridge

from building import *

cwd      = GetCurrentDir()
CPPPATH  = [cwd + '/inc']
CPPPATH += [cwd + '/porting/rt-thread']

src = Split('''
src/agile_upgrade.c
src/agile_upgrade_crc.c
''')

if GetDepend(['AGILE_UPGRADE_ENABLE_AES']):
    src += ['src/agile_upgrade_aes.c']

if GetDepend(['AGILE_UPGRADE_ENABLE_FASTLZ']):
    src += ['src/agile_upgrade_fastlz.c']

if GetDepend(['AGILE_UPGRADE_ENABLE_QUICKLZ']):
    src += ['src/agile_upgrade_quicklz.c']

if GetDepend(['AGILE_UPGRADE_ENABLE_FAL']):
    src += ['adapter/agile_upgrade_fal.c']

if GetDepend(['AGILE_UPGRADE_ENABLE_FILE']):
    src += ['adapter/agile_upgrade_file.c']

group = DefineGroup('agile_upgrade', src, depend = ['PKG_USING_AGILE_UPGRADE'], CPPPATH = CPPPATH)

Return('group')
