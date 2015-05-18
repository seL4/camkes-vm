/*#
 *# Copyright 2014, NICTA
 *#
 *# This software may be distributed and modified according to the terms of
 *# the BSD 2-Clause license. Note that NO WARRANTY is provided.
 *# See "LICENSE_BSD2.txt" for details.
 *#
 *# @TAG(NICTA_BSD)
 #*/

#include <sel4/sel4.h>
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sync/sem-bare.h>
#include <camkes/marshal.h>
#include <camkes/dataport.h>
#include <camkes/error.h>
#include <camkes/tls.h>

/*? macros.show_includes(me.from_instance.type.includes) ?*/
/*? macros.show_includes(me.from_interface.type.includes, '../static/components/' + me.from_instance.type.name + '/') ?*/

/*- set suffix = "_buf" -*/
/*- include 'seL4MultiSharedData-from.template.c' -*/

/*- set ep = alloc('ep', seL4_EndpointObject, write=True, grant=True) -*/
/*- set badge = configuration[me.from_instance.name].get('%s_attributes' % me.from_interface.name) -*/
/*- if badge is not none -*/
    /*- set badge = badge.strip('"') -*/
    /*- do cap_space.cnode[ep].set_badge(int(badge, 0)) -*/
/*- endif -*/

/*- set base = me.from_interface.name + "_buf" -*/

/*- set methods_len = len(me.from_interface.type.methods) -*/
/*- set instance = me.from_instance.name -*/
/*- set interface = me.from_interface.name -*/

/* Interface-specific error handling */
/*- set error_handler = '%s_error_handler' % me.from_interface.name -*/
/*- include 'error-handler.c' -*/

/*- include 'array-typedef-check.c' -*/

/*- for i, m in enumerate(me.from_interface.type.methods) -*/

/*- set name = m.name -*/
/*- set function = '%s_marshal_inputs' % m.name -*/
/*- set buffer = base -*/
/*- set size = 'PAGE_SIZE_4K' -*/
/*- set method_index = i -*/
/*- set input_parameters = filter(lambda('x: x.direction in [\'refin\', \'in\', \'inout\']'), m.parameters) -*/
/*- include 'marshal-inputs.c' -*/

/*- set function = '%s_unmarshal_outputs' % m.name -*/
/*- set output_parameters = filter(lambda('x: x.direction in [\'out\', \'inout\']'), m.parameters) -*/
/*- set return_type = m.return_type -*/
/*- set allow_trailing_data = true -*/
/*- include 'unmarshal-outputs.c' -*/

/*- set ret_tls_var = c_symbol('ret_tls_var_from') -*/
/*- if m.return_type -*/
  /*# We will need to take the address of a value representing this return
   *# value at some point. Construct a TLS variable.
   #*/
  /*- set name = ret_tls_var -*/
  /*- if m.return_type.array -*/
    /*- if isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
      /*- set array = False -*/
      /*- set type = 'char**' -*/
      /*- include 'thread_local.c' -*/
    /*- else -*/
      /*- set array = False -*/
      /*- set type = '%s*' % show(m.return_type) -*/
      /*- include 'thread_local.c' -*/
    /*- endif -*/
  /*- elif isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
    /*- set array = False -*/
    /*- set type = 'char*' -*/
    /*- include 'thread_local.c' -*/
  /*- else -*/
    /*- set array = False -*/
    /*- set type = show(m.return_type) -*/
    /*- include 'thread_local.c' -*/
  /*- endif -*/
/*- endif -*/

/*- if m.return_type -*/
    /*? show(m.return_type) ?*/
/*- else -*/
    void
/*- endif -*/
/*? me.from_interface.name ?*/_/*? m.name ?*/(
/*- set ret_sz = c_symbol('ret_sz') -*/
/*- if m.return_type and m.return_type.array -*/
    size_t * /*? ret_sz ?*/
    /*- if len(m.parameters) > 0 -*/
        ,
    /*- endif -*/
/*- endif -*/
/*- for p in m.parameters -*/
  /*- if p.direction == 'in' -*/
    /*- if p.array -*/
      size_t /*? p.name ?*/_sz,
      /*- if isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
        char **
      /*- else -*/
        const /*? show(p.type) ?*/ *
      /*- endif -*/
    /*- elif isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
      const char *
    /*- else -*/
      /*? show(p.type) ?*/
    /*- endif -*/
    /*? p.name ?*/
  /*- else -*/
    /*? assert(p.direction in ['refin', 'out', 'inout']) ?*/
    /*- if p.array -*/
      /*- if p.direction == 'refin' -*/
        const
      /*- endif -*/
      size_t * /*? p.name ?*/_sz,
      /*- if isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
        char ***
      /*- else -*/
        /*? show(p.type) ?*/ **
      /*- endif -*/
    /*- elif isinstance(p.type, camkes.ast.Type) and p.type.type == 'string' -*/
      char **
    /*- else -*/
      /*- if p.direction == 'refin' -*/
        const
      /*- endif -*/
      /*? show(p.type) ?*/ *
    /*- endif -*/
    /*? p.name ?*/
  /*- endif -*/
  /*- if not loop.last -*/
    ,
  /*- endif -*/
/*- endfor -*/
/*- if (m.return_type is none or not m.return_type.array) and len(m.parameters) == 0 -*/
  void
/*- endif -*/
) {
    /*- set ret_val = c_symbol('return') -*/
    /*- set ret_ptr = c_symbol('return_ptr') -*/
    /*- if m.return_type -*/
      /*- if m.return_type.array -*/
        /*- if isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
          char ** /*? ret_val ?*/ UNUSED;
          char *** /*? ret_ptr ?*/ = TLS_PTR(/*? ret_tls_var ?*/, /*? ret_val ?*/);
        /*- else -*/
          /*? show(m.return_type) ?*/ * /*? ret_val ?*/ UNUSED;
          /*? show(m.return_type) ?*/ ** /*? ret_ptr ?*/ = TLS_PTR(/*? ret_tls_var ?*/, /*? ret_val ?*/);
        /*- endif -*/
      /*- elif isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string' -*/
        char * /*? ret_val ?*/ UNUSED;
        char ** /*? ret_ptr ?*/ = TLS_PTR(/*? ret_tls_var ?*/, /*? ret_val ?*/);
      /*- else -*/
        /*? show(m.return_type) ?*/ /*? ret_val ?*/ UNUSED;
        /*? show(m.return_type) ?*/ * /*? ret_ptr ?*/ = TLS_PTR(/*? ret_tls_var ?*/, /*? ret_val ?*/);
      /*- endif -*/
    /*- endif -*/

    /* Marshal all the parameters */
    /*- set function = '%s_marshal_inputs' % m.name -*/
    /*- set length = c_symbol('length') -*/
    unsigned int /*? length ?*/ = /*- include 'call-marshal-inputs.c' -*/;
    if (unlikely(/*? length ?*/ == UINT_MAX)) {
        /* Error in marshalling; bail out. */
        /*- if m.return_type -*/
            /*- if m.return_type.array or (isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string')  -*/
                return NULL;
            /*- else -*/
                memset(/*? ret_ptr ?*/, 0, sizeof(* /*? ret_ptr ?*/));
                return * /*? ret_ptr ?*/;
            /*- endif -*/
        /*- else -*/
            return;
        /*- endif -*/
    }

    /* Call the endpoint */
    /*- set info = c_symbol('info') -*/
    seL4_MessageInfo_t /*? info ?*/ = seL4_MessageInfo_new(0, 0, 0, 0);
    /*? info ?*/ = seL4_Call(/*? ep ?*/, /*? info ?*/);

    /*- set size = c_symbol('size') -*/
    unsigned int /*? size ?*/ = PAGE_SIZE_4K;

    /* Unmarshal the response */
    /*- set function = '%s_unmarshal_outputs' % m.name -*/
    /*- set return_type = m.return_type -*/
    /*- set err = c_symbol('error') -*/
    int /*? err ?*/ = /*- include 'call-unmarshal-outputs.c' -*/;
    if (unlikely(/*? err ?*/ != 0)) {
        /* Error in unmarshalling; bail out. */
        /*- if m.return_type -*/
            /*- if m.return_type.array or (isinstance(m.return_type, camkes.ast.Type) and m.return_type.type == 'string')  -*/
                return NULL;
            /*- else -*/
                memset(/*? ret_ptr ?*/, 0, sizeof(* /*? ret_ptr ?*/));
                return * /*? ret_ptr ?*/;
            /*- endif -*/
        /*- else -*/
            return;
        /*- endif -*/
    }

    /*- if m.return_type -*/
        return * /*? ret_ptr ?*/;
    /*- endif -*/
}
/*- endfor -*/
