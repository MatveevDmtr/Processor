#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

const size_t MAX_LEN_LABEL_NAME = 10;


typedef struct LABEL
{
    int   Value                   ;
    char  Name[MAX_LEN_LABEL_NAME];
}
label_field;


#define allocate_array(type, num_elems)                             \
({                                                                  \
    type* location__ = (type*) calloc (num_elems, sizeof (type));   \
    log ("Type_calloc: %s\n", #type);                               \
    log ("Sizeof type: %d\n", sizeof (type*));                      \
    Assert(location__ == NULL);                                     \
    log ("Calloc returned: %p\n\n", location__);                    \
    location__;                                                     \
})


#define safe_def_int(func, invalid_value)     \
({                                            \
    int variable__ = func;                    \
    Assert(variable__ == invalid_value);      \
    variable__;                               \
})

#endif
