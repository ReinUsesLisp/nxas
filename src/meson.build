nxas_lib = static_library('nxas_lib', sources : [
	'context.cpp',
    'parse.cpp',
    'error.cpp',
    'token.cpp',
    'helper.cpp',
    'write.cpp',
    'dksh.cpp',
    'options.cpp'
])

nxas_dep = declare_dependency(
    link_with : [nxas_lib],
    include_directories : './'
)

executable('nxas', install : true, sources : 'nxas.cpp', dependencies : nxas_dep)
