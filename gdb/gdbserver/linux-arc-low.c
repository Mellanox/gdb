/* GNU/Linux/ARC specific low level interface, for the remote server for GDB.
   Copyright 1995, 1996, 1998, 1999, 2000, 2001, 2002, 2003, 2004
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   --------------------------------------------------------------------------

   The comments within this file are also licensed under under the terms of
   the GNU Free Documentation License as published by the Free Software
   Foundation; either version 1.3 of the License, or (at your option) any
   later version. See the file fdi.texi in the gdb/doc directory for copying
   conditions.

   You should have received a copy of the GNU Free Documentation License along
   with this program. If not, see <http://www.gnu.org/licenses/>.  */

/* -------------------------------------------------------------------------- */
/*!@file
                                                                               
   # ARC Linux gdbserver

   This file defines the implementation of gdbserver for ARC Linux.

   Access to registers via ptrace has changed between Linux 3.2 and Linux 3.9.
   With the new (ABI v3) interface in Linux 3.9, registers are accessed as a
   complete set with PTRACE_GETREGSET and PTRACE_SETREGSET. This file
   implements support only for ABI v3. If you need support for ABI v2, use
   ARC GNU Toolchain 4.8-R3 release.

   ptrace returns regsets in a struct, "user_regs_struct", defined in the
   asm/ptrace.h header. The implementation uses the arc_regsets
   functionality to control the access to registers. */
/* -------------------------------------------------------------------------- */

#include "server.h"
#include "regdef.h"
#include "linux-low.h"

#include <linux/elf.h>
#include <arpa/inet.h>
#include <asm/ptrace.h>
#include <sys/ptrace.h>

/* From include/uapi/asm/ptrace.h in the ARC Linux sources.
 * See user_regs_struct.
 *
 * @todo This is shared with arc-linux-tdep.h, so should be moved to common/
 * directory.
 */
#define BYTES_IN_REGISTER (4)
#define REGISTER_NOT_PRESENT (-1)
#define ARC_NUM_REGS (87)
static int arc_regmap[] =
{
  22 * BYTES_IN_REGISTER,	/* r0       */
  21 * BYTES_IN_REGISTER,	/* r1       */
  20 * BYTES_IN_REGISTER,	/* r2       */
  19 * BYTES_IN_REGISTER,	/* r3       */
  18 * BYTES_IN_REGISTER,	/* r4       */
  17 * BYTES_IN_REGISTER,	/* r5       */
  16 * BYTES_IN_REGISTER,	/* r6       */
  15 * BYTES_IN_REGISTER,	/* r7       */
  14 * BYTES_IN_REGISTER,	/* r8       */
  13 * BYTES_IN_REGISTER,	/* r9       */
  12 * BYTES_IN_REGISTER,	/* r10      */
  11 * BYTES_IN_REGISTER,	/* r11      */
  10 * BYTES_IN_REGISTER,	/* r12      */
  37 * BYTES_IN_REGISTER,	/* r13      */
  36 * BYTES_IN_REGISTER,	/* r14      */
  35 * BYTES_IN_REGISTER,	/* r15      */
  34 * BYTES_IN_REGISTER,	/* r16      */
  33 * BYTES_IN_REGISTER,	/* r17      */
  32 * BYTES_IN_REGISTER,	/* r18      */
  31 * BYTES_IN_REGISTER,	/* r19      */
  30 * BYTES_IN_REGISTER,	/* r20      */
  29 * BYTES_IN_REGISTER,	/* r21      */
  28 * BYTES_IN_REGISTER,	/* r22      */
  27 * BYTES_IN_REGISTER,	/* r23      */
  26 * BYTES_IN_REGISTER,	/* r24      */
  25 * BYTES_IN_REGISTER,	/* r25      */
  9 * BYTES_IN_REGISTER,	/* r26 (gp) */
  8 * BYTES_IN_REGISTER,	/* fp       */
  23 * BYTES_IN_REGISTER,	/* sp       */
  REGISTER_NOT_PRESENT,  	/* ilink1   */
  REGISTER_NOT_PRESENT,  	/* ilink2   */
  7 * BYTES_IN_REGISTER,	/* blink    */

  REGISTER_NOT_PRESENT,  	/* r32      */
  REGISTER_NOT_PRESENT,  	/* r33      */
  REGISTER_NOT_PRESENT,  	/* r34      */
  REGISTER_NOT_PRESENT,  	/* r35      */
  REGISTER_NOT_PRESENT,  	/* r36      */
  REGISTER_NOT_PRESENT,  	/* r37      */
  REGISTER_NOT_PRESENT,  	/* r38      */
  REGISTER_NOT_PRESENT,  	/* r39      */
  REGISTER_NOT_PRESENT,  	/* r40      */
  REGISTER_NOT_PRESENT,  	/* r41      */
  REGISTER_NOT_PRESENT,  	/* r42      */
  REGISTER_NOT_PRESENT,  	/* r43      */
  REGISTER_NOT_PRESENT,  	/* r44      */
  REGISTER_NOT_PRESENT,  	/* r45      */
  REGISTER_NOT_PRESENT,  	/* r46      */
  REGISTER_NOT_PRESENT,  	/* r47      */
  REGISTER_NOT_PRESENT,  	/* r48      */
  REGISTER_NOT_PRESENT,  	/* r49      */
  REGISTER_NOT_PRESENT,  	/* r50      */
  REGISTER_NOT_PRESENT,  	/* r51      */
  REGISTER_NOT_PRESENT,  	/* r52      */
  REGISTER_NOT_PRESENT,  	/* r53      */
  REGISTER_NOT_PRESENT,  	/* r54      */
  REGISTER_NOT_PRESENT,  	/* r55      */
  REGISTER_NOT_PRESENT,  	/* r56      */
  REGISTER_NOT_PRESENT,  	/* r57      */
  REGISTER_NOT_PRESENT,  	/* r58      */
  REGISTER_NOT_PRESENT,  	/* r59      */
  4 * BYTES_IN_REGISTER,	/* lp_count */
  REGISTER_NOT_PRESENT,  	/* reserved */
  REGISTER_NOT_PRESENT,  	/* limm     */
  REGISTER_NOT_PRESENT,  	/* pcl      */

  39 * BYTES_IN_REGISTER,	/* stop_pc  */
  2 * BYTES_IN_REGISTER,	/* lp_start */
  3 * BYTES_IN_REGISTER,	/* lp_end   */
  5 * BYTES_IN_REGISTER,	/* status32 */
  REGISTER_NOT_PRESENT,  	/* st32_l1  */
  REGISTER_NOT_PRESENT,  	/* st32_l2  */
  REGISTER_NOT_PRESENT,  	/* irq_lv12 */
  REGISTER_NOT_PRESENT,  	/* irq_lev  */
  REGISTER_NOT_PRESENT,  	/* irq_hint */
  6 * BYTES_IN_REGISTER,	/* ret      */
  REGISTER_NOT_PRESENT,  	/* erbta    */
  REGISTER_NOT_PRESENT,  	/* erstatus */
  REGISTER_NOT_PRESENT,  	/* ecr      */
  38 * BYTES_IN_REGISTER,	/* efa      */
  REGISTER_NOT_PRESENT,  	/* icause1  */
  REGISTER_NOT_PRESENT,  	/* icause2  */
  REGISTER_NOT_PRESENT,  	/* ienable  */
  REGISTER_NOT_PRESENT,  	/* itrigger */
  1 * BYTES_IN_REGISTER,	/* bta      */
  REGISTER_NOT_PRESENT,  	/* bta_l1   */
  REGISTER_NOT_PRESENT,  	/* bta_l2   */
  REGISTER_NOT_PRESENT,  	/* irq pulse*/
  REGISTER_NOT_PRESENT,  	/* irq pend */

  /* 24 * BYTES_IN_REGISTER,	orig_r8  */
};

/* -------------------------------------------------------------------------- */
/*				  Data types				      */
/* -------------------------------------------------------------------------- */

#ifdef ARC_LEGACY_PTRACE_ABI
/*! Register data structure.

    This struct matches the sequence returned by ptrace.

    The meaning of orig_r0 is lost in the mists of time. orig_r8 tells you
    what stop_pc, status32 and ret really refer to, but is dropped in ABI v3
    and so not used here. */
struct user_regs_struct {

  struct pt_regs  scratch;
  struct callee_regs  callee;
  long int efa;		/* break pt addr, for break points in delay slots */
  long int stop_pc;	/* give dbg stop_pc directly after checking orig_r8 */
};
#endif

/* Debug registers change state */
enum dr_changed_state
  {
    dr_unchanged    = 0,
    dr_changed      = 1,
    dr_new_thread   = 2,
  };

#if __BYTE_ORDER == __BIG_ENDIAN
#define DR_TYPE_SHIFT 4
#define DR_LEN_SHIFT  24
#else
#define DR_TYPE_SHIFT 26
#define DR_LEN_SHIFT  0
#endif

/* Watchpoint/breakpoint read/write values in control register.
   The left shift is according to the control structure */
#define DR_TYPE_EXECUTE (0x0 << DR_TYPE_SHIFT)	/* Break on instruction execution. */
#define DR_TYPE_WRITE   (0x1 << DR_TYPE_SHIFT)	/* Break on data writes. */
#define DR_TYPE_READ    (0x2 << DR_TYPE_SHIFT)	/* Break on data reads. */
#define DR_TYPE_ACCESS  (0x3 << DR_TYPE_SHIFT)	/* Break on data reads or writes. */

/* Watchpoint/breakpoint length fields in control register.
   The left shift is according to the control structure  */
#define DR_LEN_1        (0x1 << DR_LEN_SHIFT) /* 1-byte.	*/
#define DR_LEN_2        (0x2 << DR_LEN_SHIFT) /* 2-byte.  */
#define DR_LEN_4        (0x4 << DR_LEN_SHIFT) /* 4-byte.  */
#define DR_LEN_8        (0x8 << DR_LEN_SHIFT) /* 8-byte.  */

/* Since we cannot dynamically allocate subfields of arch_process_info,
   assume a maximum number of supported break-/watchpoints.  */
#define ARC_MAX_HBP_SLOTS 8

/* A macro to loop over all debug registers.  */
#define ALL_DEBUG_REGISTERS(i) for (i = 0; i < ARC_MAX_HBP_SLOTS; i++)

/* The I'th debug register is vacant if its Debug Control
 * register is reset.  */
#define ARC_DR_VACANT(state, i) (state->control[i] == 0)

/* Structure used to keep track of hardware break-/watch-points.  */
struct arc_debug_reg_state
{
  /* Address to break on, or being watched.  */
  unsigned int address[ARC_MAX_HBP_SLOTS];
  /* Control register for break-/watch- point.  */
  unsigned int control[ARC_MAX_HBP_SLOTS];
  /* Reference counts for each debug register.  */
  int ref_count[ARC_MAX_HBP_SLOTS];
};

/* Per-process arch-specific data we want to keep.  */
struct arch_process_info
{
  struct arc_debug_reg_state debug_reg_state;
};

/* Per-thread arch-specific data we want to keep.  */
struct arch_lwp_info
{
  /* Non-zero if our copy differs from what's recorded in the thread.  */
  enum dr_changed_state debug_register_changed[ARC_MAX_HBP_SLOTS];
  /* Cached stopped data address.  */
  CORE_ADDR stopped_data_address;
};

/* -------------------------------------------------------------------------- */
/*		    Static variables global to this file.		      */
/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */
/*			Externally defined functions.			      */
/* -------------------------------------------------------------------------- */

/*! Defined in auto-generated file arc-reg.c. */
extern void init_registers_arc (void);
extern const struct target_desc *tdesc_arc;


/* -------------------------------------------------------------------------- */
/*			       Target functions				      */
/*									      */
/* Presented in the order they are used in the_low_target.		      */
/* -------------------------------------------------------------------------- */

/*! Return target description.
 *
 *  @todo This function should return appropriate tdesc (ARCompact or ARC v2),
 *  depending on what target it is run on.
 */
static const struct target_desc *
arc_read_description (void)
{
  return tdesc_arc;
}	/* arc_read_description () */


static void
arc_arch_setup (void)
{
  current_process ()->tdesc = arc_read_description ();
}


/*! Can a register be read?

    We can only write the core registers excluding ilink1, ilink2, the
    extension registers, reserved register and limm registers along with
    the lp_start, lp_end, status32 and efa aux registers.

    @note The EFA register is a bit odd, since it is a privileged register,
          but it will help deal with breakpoints in delay slots.

    @todo This is not terribly efficient, but that probably doesn't matter. If
          it ever did matter, we should use the init function to build a table
          of whether regs can be read or written, then this function is just a
          table lookup.

    @param[in] regno  Number of reg to check.
    @return           Non-zero if register cannot be read, zero otherwise. */
static int
arc_cannot_fetch_register (int regno)
{
  return (regno >= ARC_NUM_REGS || regno < 0 || arc_regmap[regno] == -1);
}	/* arc_cannot_fetch_register () */


/*! Can a register be written?

    We can only write the core registers excluding ilink1, ilink2, the
    extension registers, reserved register, limm and PCL registers along with
    the lp_start and lp_end aux registers.

    @todo This is not terribly efficient, but that probably doesn't matter. If
          it ever did matter, we should use the init function to build a table
          of whether regs can be read or written, then this function is just a
          table lookup.

    @param[in] regno  Number of reg to check.
    @return           Non-zero if register cannot be written, zero otherwise. */
static int
arc_cannot_store_register (int regno)
{
  return (regno >= ARC_NUM_REGS || regno < 0 || arc_regmap[regno] == -1);
}	/* arc_cannot_store_register () */


/*! Get the PC from the register cache.

    @param[in] regcache  Register cache
    @return              The value of the PC. */
static CORE_ADDR
arc_get_pc (struct regcache *regcache)
{
  unsigned long pc;
  collect_register_by_name (regcache, "pc", &pc);
  if (debug_threads)
    fprintf (stderr, "pc is %08lx\n", pc);
  return (CORE_ADDR) pc;

}	/* arc_get_pc () */


/*! Set the PC in the register cache.

    @param[out] regcache  Register cache
    @param[in]  pc        Value to write to the PC. */
static void
arc_set_pc (struct regcache *regcache, CORE_ADDR pc)
{
  unsigned long newpc = (unsigned long) pc;

  supply_register_by_name (regcache, "pc", &newpc);

}	/* arc_set_pc () */

/* Clear the reference counts and forget everything we knew about the
   debug registers.  */
void
arc_low_init_dregs (struct arc_debug_reg_state *state)
{
  int i;

  ALL_DEBUG_REGISTERS (i)
  {
    state->address[i] = 0;
    state->control[i] = 0;
    state->ref_count[i] = 0;
  }
}

/* Called when a new process is created.  */

static struct arch_process_info *
arc_new_process (void)
{
  struct arch_process_info *info = xcalloc (1, sizeof (*info));

  arc_low_init_dregs (&info->debug_reg_state);

  return info;
}

/* Called when a new thread is detected.  */

static struct arch_lwp_info *
arc_new_thread (void)
{
  int i;
  struct arch_lwp_info *info = xcalloc (1, sizeof (*info));

  ALL_DEBUG_REGISTERS(i)
  {
    /* mark changed status differently so it can be ignored
     for zero referenced slots */
    info->debug_register_changed[i] = dr_new_thread;
  }

  return info;
}

/*! Determine if we have a breakpoint at an address.

    The breakpoint instruction is TRAP 1. We can use the network function
    ntohs to convert from its little-endian form (0x3e78) to the host
    representation, which may be little-endian or big-endian (remember network
    representation is defined to be little-endian).

    @param[in] where  The address to look at.
    @return           Non-zero (TRUE) if there is a breakpoint at the address,
                      zero (FALSE) otherwise. */
static int
arc_breakpoint_at (CORE_ADDR where)
{
  uint16_t insn;
  uint16_t breakpoint = ntohs (0x3e78);

  (*the_target->read_memory) (where, (unsigned char *) &insn, 2);
  return insn == breakpoint;

}	/* arc_breakpoint_at () */

#define Z_PACKET_HBP_EXECUTE	'1'
#define Z_PACKET_HBP_WRITE	'2'
#define Z_PACKET_HBP_READ	'3'
#define Z_PACKET_HBP_ACCESS	'4'

/* Map the protocol hw breakpoint type TYPE to enum target_hw_bp_type.  */
static enum target_hw_bp_type Z_packet_to_hw_type (char type)
{
  switch (type) {
  case Z_PACKET_HBP_EXECUTE:
    return hw_execute;
  case Z_PACKET_HBP_WRITE:
    return hw_write;
  case Z_PACKET_HBP_READ:
    return hw_read;
  case Z_PACKET_HBP_ACCESS:
    return hw_access;
  default:
    error ("Z_packet_to_hw_type: bad watchpoint type %c", type);
  }
}

static unsigned
arc_length_and_rw_bits (int len, enum target_hw_bp_type type)
{
  unsigned rw;

  switch (type) {
  case hw_write:
    rw = DR_TYPE_WRITE;
    break;
  case hw_read:
    rw = DR_TYPE_READ;
    break;
  case hw_access:
    rw = DR_TYPE_ACCESS;
    break;
  case hw_execute:
    rw = DR_TYPE_EXECUTE;
    break;
  default:
    error ("\
Invalid hardware breakpoint type %d in arc_length_and_rw_bits.\n", (int) type);
  }

  switch (len) {
  case 1:
    rw = (DR_LEN_1 | rw);
    break;
  case 2:
    rw = (DR_LEN_2 | rw);
    break;
  case 4:
    rw = (DR_LEN_4 | rw);
    break;
  case 8:
    rw = (DR_LEN_8 | rw);
    break;
  default:
    error ("\
Invalid hardware breakpoint len %d in arc_length_and_rw_bits.\n", (int) len);
  }

  return rw;
}

/* Insert a hw breakpoint at address ADDR, which is assumed to be aligned
   according to the length of the region to watch. Return 0 on
   success, -1 on failure.  */
static int
arc_insert_aligned_hbp (struct arc_debug_reg_state *state,
                        CORE_ADDR addr, unsigned len_rw_bits)
{
  int i;

  /* First, look for an occupied debug register with the same address
     and the same RW and LEN definitions.  If we find one, we can
     reuse it for this hw breakpoint as well (and save a register).  */
  ALL_DEBUG_REGISTERS (i)
  {
    if (!ARC_DR_VACANT (state, i)
        && state->address[i] == addr
        && (state->control[i] == len_rw_bits))
    {
      state->ref_count[i]++;
      return 0;
    }
  }

  /* Next, look for a vacant debug register.  */
  ALL_DEBUG_REGISTERS (i)
  {
    if (ARC_DR_VACANT (state, i))
      break;
  }

  /* No more debug registers!  */
  if (i >= ARC_MAX_HBP_SLOTS)
    return -1;

  /* Now set up the register I to watch our region.  */
  state->address[i] = addr;
  state->ref_count[i] = 1;
  state->control[i] = len_rw_bits;

  return 0;
}

static int
update_debug_registers_callback (struct inferior_list_entry *entry,
                                 void *regnum)
{
  struct thread_info *thr = (struct thread_info *) entry;
  struct lwp_info *lwp = get_thread_lwp (thr);
  int pid = pid_of (current_thread);
  int i = *(int*)regnum;

  /* Only update the threads of this process.  */
  if (pid_of (thr) == pid)
  {
    /* The actual update is done later just before resuming the lwp,
       we just mark that the registers need updating.  */
    lwp->arch_private->debug_register_changed[i] = dr_changed;

    /* If the lwp isn't stopped, force it to momentarily pause, so
       we can update its debug registers.  */
    if (!lwp->stopped)
      linux_stop_lwp (lwp);
  }

  return 0;
}

/* Update the inferior's debug register REGNUM from STATE.  */
void
arc_dr_low_set (const struct arc_debug_reg_state *state, int regnum)
{
  if (! (regnum >= 0 && regnum <= ARC_MAX_HBP_SLOTS))
    error ("Invalid debug register %d", regnum);

  find_inferior (&all_threads, update_debug_registers_callback, &regnum);
}

/* Update the inferior debug registers state, in INF_STATE, with the
   new debug registers state, in NEW_STATE.  */
static void
arc_update_inferior_debug_regs (struct arc_debug_reg_state *inf_state,
                                struct arc_debug_reg_state *new_state)
{
  int i;
  ALL_DEBUG_REGISTERS (i)
  {
    if (ARC_DR_VACANT (new_state, i) != ARC_DR_VACANT (inf_state, i))
    {
      arc_dr_low_set (new_state, i);
    }
    else
    {
      gdb_assert (new_state->address[i] == inf_state->address[i]);
      gdb_assert (new_state->control[i] == inf_state->control[i]);
    }
  }

  *inf_state = *new_state;
}

/* Insert a hw breakpoint.  Return 0 on success, -1 on failure.  */
static int arc_low_insert_hbp (struct arc_debug_reg_state *state,
                               char type_from_packet, CORE_ADDR addr, int len)
{
  int retval;
  enum target_hw_bp_type type = Z_packet_to_hw_type (type_from_packet);
  /* Work on a local copy of the debug registers, and on success,
     commit the change back to the inferior.  */
  struct arc_debug_reg_state local_state = *state;

  if ((len != 1 && len != 2 && len != 4) || addr % len != 0)
    return -1; /* nonaligned */
  else
  {
    unsigned len_rw = arc_length_and_rw_bits (len, type);
    retval = arc_insert_aligned_hbp (&local_state, addr, len_rw);
  }

  if (retval == 0)
    arc_update_inferior_debug_regs (state, &local_state);

  return retval;
}

/* Breakpoint/Watchpoint support.  */
static int
arc_insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
                  int len, struct raw_breakpoint *bp)
{
  struct process_info *proc = current_process ();

  switch (type)
  {
  case raw_bkpt_type_hw:
  case raw_bkpt_type_write_wp:
  case raw_bkpt_type_read_wp:
  case raw_bkpt_type_access_wp:
    return arc_low_insert_hbp (&proc->private->arch_private->debug_reg_state,
                               type, addr, len);
  default:
    /* Unsupported.  */
    return 1;
  }
}

static void
arc_dr_set (ptid_t ptid, int regnum, unsigned long addr, unsigned long ctrl)
{
  int tid;

  tid = ptid_get_lwp (ptid);

  if (ctrl) /* set address only if ctrl is initialized */
  {
    errno = 0;
    ptrace (PTRACE_POKEUSER, tid,
            offsetof (struct user, u_dr_value[regnum]), addr);
    if (errno != 0)
      error ("Couldn't write debug register value");
  }

  errno = 0;
  ptrace (PTRACE_POKEUSER, tid,
          offsetof (struct user, u_dr_control[regnum]), ctrl);
  if (errno != 0)
    error ("Couldn't write debug register control");
}

/* Called when resuming a thread.
   If the debug regs have changed, update the thread's copies.  */
static void
arc_prepare_to_resume (struct lwp_info *lwp)
{
  ptid_t ptid = ptid_of (get_lwp_thread (lwp));
  int i;
  int pid = ptid_get_pid (ptid);
  struct process_info *proc = find_process_pid (pid);
  struct arc_debug_reg_state *state =
                   &proc->private->arch_private->debug_reg_state;

  ALL_DEBUG_REGISTERS (i)
  {
    if (lwp->arch_private->debug_register_changed[i])
    {
      if (lwp->arch_private->debug_register_changed[i] == dr_changed ||
         (lwp->arch_private->debug_register_changed[i] == dr_new_thread &&
          state->ref_count[i] > 0))
        arc_dr_set (ptid, i, state->address[i], state->control[i]);

      lwp->arch_private->debug_register_changed[i] = dr_unchanged;
    }
  }

}

/* Remove a hw breakpoint at address ADDR, which is assumed to be aligned
   according to the length of the region to watch. Return 0 on
   success, -1 on failure.  */
static int
arc_remove_aligned_hbp (struct arc_debug_reg_state *state,
                        CORE_ADDR addr, unsigned len_rw_bits)
{
  int i, retval = -1;

  ALL_DEBUG_REGISTERS (i)
  {
    if (!ARC_DR_VACANT (state, i)
        && state->address[i] == addr
        && (state->control[i] == len_rw_bits))
    {
      if (--state->ref_count[i] == 0) /* No longer in use?  */
      {
        /* Reset */
        state->address[i] = 0;
        state->control[i] = 0;
      }
      retval = 0;
    }
  }

  return retval;
}

/* Remove a hw breakpoint that watched the memory region which starts at
   address ADDR, whose length is LEN bytes, and for accesses of the
   type TYPE_FROM_PACKET.  Return 0 on success, -1 on failure.  */

int
arc_low_remove_hbp (struct arc_debug_reg_state *state,
                    char type_from_packet, CORE_ADDR addr, int len)
{
  int retval;
  enum target_hw_bp_type type = Z_packet_to_hw_type (type_from_packet);
  /* Work on a local copy of the debug registers, and on success,
     commit the change back to the inferior.  */
  struct arc_debug_reg_state local_state = *state;
  unsigned len_rw = arc_length_and_rw_bits (len, type);

  retval = arc_remove_aligned_hbp (&local_state, addr, len_rw);

  if (retval == 0)
    arc_update_inferior_debug_regs (state, &local_state);

  return retval;
}

/* Breakpoint/Watchpoint support.  */
static int
arc_remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
                  int len, struct raw_breakpoint *bp)
{
  struct process_info *proc = current_process ();

  switch (type)
  {
  case raw_bkpt_type_hw:
  case raw_bkpt_type_write_wp:
  case raw_bkpt_type_read_wp:
  case raw_bkpt_type_access_wp:
    return arc_low_remove_hbp (&proc->private->arch_private->debug_reg_state,
                               type, addr, len);
  default:
    /* Unsupported.  */
    return 1;
  }
}

/* Return whether current thread is stopped due to a watchpoint.  */
static int
arc_stopped_by_watchpoint (void)
{
  struct lwp_info *lwp = get_thread_lwp (current_thread);
  siginfo_t siginfo;

  /* Retrieve siginfo.  */
  errno = 0;
  ptrace (PTRACE_GETSIGINFO, lwpid_of (current_thread), 0, &siginfo);
  if (errno != 0)
    return 0;

  /* This must be a hardware breakpoint.  */
  if (siginfo.si_signo != SIGTRAP
      || (siginfo.si_code & 0xffff) != 0x0004 /* TRAP_HWBKPT */)
    return 0;

  /* If we are in a negative slot then we're looking at a breakpoint and not
       a watchpoint.  */
  if (siginfo.si_errno <= 0)
    return 0;

  /* Cache stopped data address for use by arc_stopped_data_address.  */
  lwp->arch_private->stopped_data_address
    = (CORE_ADDR) (uintptr_t) siginfo.si_addr;

  return 1;
}

/* Return data address that triggered watchpoint.  Called only if
   arc_stopped_by_watchpoint returned true.  */
static CORE_ADDR
arc_stopped_data_address (void)
{
  struct lwp_info *lwp = get_thread_lwp (current_thread);

  return lwp->arch_private->stopped_data_address;
}

/* -------------------------------------------------------------------------- */
/*			Register set access functions.			      */
/*									      */
/* Only for ARC ABI v3. Presented in the order they are used in		      */
/* arc_regsets.							      */
/* -------------------------------------------------------------------------- */

/*! Populate a ptrace NT_PRSTATUS regset from a regcache.

    We know the buffer regset supplied must therefore be a struct
    user_regs_struct.

    @note This appears to be a unique approach to populating the buffer, but
          being name, rather than offset based, it is robust to future API
          changes. It is appropriate with the new Linux PTRACE_GETREGSET and
          PTRACE_SETREGSET functionality. Since this function should only be
          called when we are in user interaction with the client, performance
          is not critical.

    @param[in]  regcache  Register cache supplying the values.
    @param[out] buf       ptrace buffer to be filled. */
static void
arc_fill_gregset (struct regcache *regcache, void *buf)
{
  struct user_regs_struct *regbuf = buf;

  /* Collect all the unique registers by name. */
  collect_register_by_name (regcache, "r0", &(regbuf->scratch.r0));
  collect_register_by_name (regcache, "r1", &(regbuf->scratch.r1));
  collect_register_by_name (regcache, "r2", &(regbuf->scratch.r2));
  collect_register_by_name (regcache, "r3", &(regbuf->scratch.r3));
  collect_register_by_name (regcache, "r4", &(regbuf->scratch.r4));
  collect_register_by_name (regcache, "r5", &(regbuf->scratch.r5));
  collect_register_by_name (regcache, "r6", &(regbuf->scratch.r6));
  collect_register_by_name (regcache, "r7", &(regbuf->scratch.r7));
  collect_register_by_name (regcache, "r8", &(regbuf->scratch.r8));
  collect_register_by_name (regcache, "r9", &(regbuf->scratch.r9));
  collect_register_by_name (regcache, "r10", &(regbuf->scratch.r10));
  collect_register_by_name (regcache, "r11", &(regbuf->scratch.r11));
  collect_register_by_name (regcache, "r12", &(regbuf->scratch.r12));
  collect_register_by_name (regcache, "r13", &(regbuf->callee.r13));
  collect_register_by_name (regcache, "r14", &(regbuf->callee.r14));
  collect_register_by_name (regcache, "r15", &(regbuf->callee.r15));
  collect_register_by_name (regcache, "r16", &(regbuf->callee.r16));
  collect_register_by_name (regcache, "r17", &(regbuf->callee.r17));
  collect_register_by_name (regcache, "r18", &(regbuf->callee.r18));
  collect_register_by_name (regcache, "r19", &(regbuf->callee.r19));
  collect_register_by_name (regcache, "r20", &(regbuf->callee.r20));
  collect_register_by_name (regcache, "r21", &(regbuf->callee.r21));
  collect_register_by_name (regcache, "r22", &(regbuf->callee.r22));
  collect_register_by_name (regcache, "r23", &(regbuf->callee.r23));
  collect_register_by_name (regcache, "r24", &(regbuf->callee.r24));
  collect_register_by_name (regcache, "r25", &(regbuf->callee.r25));
  collect_register_by_name (regcache, "gp", &(regbuf->scratch.gp));
  collect_register_by_name (regcache, "fp", &(regbuf->scratch.fp));
  collect_register_by_name (regcache, "sp", &(regbuf->scratch.sp));
  /* ilink1 not currently supported. */
  /* ilink2 not currently supported. */
  collect_register_by_name (regcache, "blink", &(regbuf->scratch.blink));
  /* Extension core regs 32-59 not currently supported. */
  collect_register_by_name (regcache, "lp_count", &(regbuf->scratch.lp_count));
  /* reserved, limm and pcl not currently supported. */
  /* If you write the PC, that means the return address. */
  collect_register_by_name (regcache, "pc", &(regbuf->scratch.ret));
  /* Auxilliary registers */
  collect_register_by_name (regcache, "lp_start", &(regbuf->scratch.lp_start));
  collect_register_by_name (regcache, "lp_end", &(regbuf->scratch.lp_end));
  /* status32 not writable. */
  /* status32_l1 not currently supported. */
  /* status32_l2 not currently supported. */
  /* aux_irq_lv12 not currently supported. */
  /* aux_irq_lev not currently supported. */
  /* aux_irq_hint not currently supported. */
  /* eret not currently supported. */
  /* erbta not currently supported. */
  /* erstatus not currently supported. */
  /* ecr not currently supported. */
  /* efa not writable. */
  /* icause1 not currently supported. */
  /* icause2 not currently supported. */
  /* aux_ienable not currently supported. */
  /* aux_itrigger not currently supported. */
  /* bta not currently supported. */
  /* bta_l1 not currently supported. */
  /* bta_l2 not currently supported. */
  /* aux_irq_pulse_cancel not currently supported. */
  /* aux_irq_pending not currently supported. */

}	/* arc_fill_gregset () */


/*! Populate a regcache from a ptrace NT_PRSTATUS regset.

    We know the regset buffer supplied must therefore be a struct
    user_regs_struct.

    @note This appears to be a unique approach to populating the regcache, but
          being name, rather than offset based, it is robust to future API
          changes. It is appropriate with the new Linux PTRACE_GETREGSET and
          PTRACE_SETREGSET functionality. Since this function should only be
          called when we are in user interaction with the client, performance
          is not critical.

    @param[in]  regcache  Register cache supplying the values.
    @param[out] buf       ptrace buffer to be filled. */
static void
arc_store_gregset (struct regcache *regcache, const void *buf)
{
  const struct user_regs_struct *regbuf = buf;

  /* Supply all the unique registers by name. */
  supply_register_by_name (regcache, "r0", &(regbuf->scratch.r0));
  supply_register_by_name (regcache, "r1", &(regbuf->scratch.r1));
  supply_register_by_name (regcache, "r2", &(regbuf->scratch.r2));
  supply_register_by_name (regcache, "r3", &(regbuf->scratch.r3));
  supply_register_by_name (regcache, "r4", &(regbuf->scratch.r4));
  supply_register_by_name (regcache, "r5", &(regbuf->scratch.r5));
  supply_register_by_name (regcache, "r6", &(regbuf->scratch.r6));
  supply_register_by_name (regcache, "r7", &(regbuf->scratch.r7));
  supply_register_by_name (regcache, "r8", &(regbuf->scratch.r8));
  supply_register_by_name (regcache, "r9", &(regbuf->scratch.r9));
  supply_register_by_name (regcache, "r10", &(regbuf->scratch.r10));
  supply_register_by_name (regcache, "r11", &(regbuf->scratch.r11));
  supply_register_by_name (regcache, "r12", &(regbuf->scratch.r12));
  supply_register_by_name (regcache, "r13", &(regbuf->callee.r13));
  supply_register_by_name (regcache, "r14", &(regbuf->callee.r14));
  supply_register_by_name (regcache, "r15", &(regbuf->callee.r15));
  supply_register_by_name (regcache, "r16", &(regbuf->callee.r16));
  supply_register_by_name (regcache, "r17", &(regbuf->callee.r17));
  supply_register_by_name (regcache, "r18", &(regbuf->callee.r18));
  supply_register_by_name (regcache, "r19", &(regbuf->callee.r19));
  supply_register_by_name (regcache, "r20", &(regbuf->callee.r20));
  supply_register_by_name (regcache, "r21", &(regbuf->callee.r21));
  supply_register_by_name (regcache, "r22", &(regbuf->callee.r22));
  supply_register_by_name (regcache, "r23", &(regbuf->callee.r23));
  supply_register_by_name (regcache, "r24", &(regbuf->callee.r24));
  supply_register_by_name (regcache, "r25", &(regbuf->callee.r25));
  supply_register_by_name (regcache, "gp", &(regbuf->scratch.gp));
  supply_register_by_name (regcache, "fp", &(regbuf->scratch.fp));
  supply_register_by_name (regcache, "sp", &(regbuf->scratch.sp));
  /* ilink1 not currently supported. */
  /* ilink2 not currently supported. */
  supply_register_by_name (regcache, "blink", &(regbuf->scratch.blink));
  /* Extension core regs 32-59 not currently supported. */
  supply_register_by_name (regcache, "lp_count", &(regbuf->scratch.lp_count));
  /* limm, reserved and pcl not currently supported. */
  /* If you read the PC, that means the stop_pc address. */
  supply_register_by_name (regcache, "pc", &(regbuf->stop_pc));
  /* Auxilliary registers */
  supply_register_by_name (regcache, "lp_start", &(regbuf->scratch.lp_start));
  supply_register_by_name (regcache, "lp_end", &(regbuf->scratch.lp_end));
  supply_register_by_name (regcache, "status32", &(regbuf->scratch.status32));
  /* status32_l1 not currently supported. */
  /* status32_l2 not currently supported. */
  /* aux_irq_lv12 not currently supported. */
  /* aux_irq_lev not currently supported. */
  /* aux_irq_hint not currently supported. */
  /* eret not currently supported. */
  /* erbta not currently supported. */
  /* erstatus not currently supported. */
  /* ecr not currently supported. */
  supply_register_by_name (regcache, "efa", &(regbuf->efa));
  /* icause1 not currently supported. */
  /* icause2 not currently supported. */
  /* aux_ienable not currently supported. */
  /* aux_itrigger not currently supported. */
  /* bta not currently supported. */
  /* bta_l1 not currently supported. */
  /* bta_l2 not currently supported. */
  /* aux_irq_pulse_cancel not currently supported. */
  /* aux_irq_pending not currently supported. */

}	/* arc_store_gregset () */


/* Fetch the thread-local storage pointer for libthread_db. Note that this
 * function is not called from GDB, but is called from libthread_db.
 *
 * This is the same function as in arc-linux-nat.c */
ps_err_e
ps_get_thread_area (const struct ps_prochandle *ph, lwpid_t lwpid, int idx,
                    void **base)
{
    if (ptrace (PTRACE_GET_THREAD_AREA, lwpid, NULL, base) != 0)
      return PS_ERR;

    /* IDX is the bias from the thread pointer to the beginning of the thread
     * descriptor.  It has to be subtracted due to implementation quirks in
     * libthread_db.  */
    *base = (void *) ((char *)*base - idx);

    return PS_OK;
} /* ps_get_thread_area */


/* -------------------------------------------------------------------------- */
/*		   Global variables defining the interface		      */
/*									      */
/* target_regsets to define how to get registers (with ABI v3) and	      */
/* the_low_target to define the debug interface.			      */
/* -------------------------------------------------------------------------- */

/*! Define how we set/get different regsets (ABI v3 only).

    For ARC ABI v3 we only have support for NT_PRSTATUS, which will return all
    the registers in struct user_regs_struct. For ARC ABI v2, we must still
    define target_regsets, but with no entries. */
static struct regset_info arc_regsets[] =
{
  { .get_request    = PTRACE_GETREGSET,
    .set_request    = PTRACE_SETREGSET,
    .nt_type        = NT_PRSTATUS,
    .size           = sizeof (struct user_regs_struct),
    .type           = GENERAL_REGS,
    .fill_function  = arc_fill_gregset,
    .store_function = arc_store_gregset },
  { .get_request    = 0,		/* End marker */
    .set_request    = 0,
    .nt_type        = 0,
    .size           = -1,
    .type           = -1,
    .fill_function  = NULL,
    .store_function = NULL }
};

static struct regsets_info arc_regsets_info =
{
  arc_regsets, /* regsets */
  0, /* num_regsets */
  NULL, /* disabled regsets */
};

static struct usrregs_info arc_usrregs_info =
{
  ARC_NUM_REGS,
  arc_regmap, /* arc_regmap is empty for now. */
};

static struct regs_info regs_info =
{
  NULL, /* regset_bitmap */
  &arc_usrregs_info,
  &arc_regsets_info
};

static const struct regs_info *
arc_regs_info (void)
{
  return &regs_info;
}

/*! Define all the ARC specific interface functions.

    For a description of each entry see linux-low.h. We set all elements, even
    though most are not used, so we are likely to trigger an error on any API
    change.

    @note. We have to initialize with constants, so we can't use a variable
           for num_regs. We set it to zero here, and put the correct value in
           during the init function.

    @note. regmap is used only if access to individual registers is possible
           rather than just using regsets. By setting regset_bitmap to NULL we
           indicate on regsets are used, so we need not define regmap.

    @note. The ARC client relies exclusively on software breakpoints, even
           for single step, so will set breakpoints by using memory
           read/write. So we do not need the breakpoint functions. */
struct linux_target_ops the_low_target = {
  .arch_setup                       = arc_arch_setup,
  .regs_info                        = arc_regs_info,
  .cannot_fetch_register            = arc_cannot_fetch_register,
  .cannot_store_register            = arc_cannot_store_register,
  .fetch_register                   = NULL,
  .get_pc                           = arc_get_pc,
  .set_pc                           = arc_set_pc,
  .breakpoint                       = NULL,
  .breakpoint_len                   = 2,
  .breakpoint_reinsert_addr         = NULL,
  .decr_pc_after_break              = 0,
  .breakpoint_at                    = arc_breakpoint_at,
  .insert_point                     = arc_insert_point,
  .remove_point                     = arc_remove_point,
  .stopped_by_watchpoint            = arc_stopped_by_watchpoint,
  .stopped_data_address             = arc_stopped_data_address,
  .collect_ptrace_register          = NULL,
  .supply_ptrace_register           = NULL,
  .siginfo_fixup                    = NULL,
  .new_process                      = arc_new_process,
  .new_thread                       = arc_new_thread,
  .prepare_to_resume                = arc_prepare_to_resume,
  .process_qsupported               = NULL,
  .supports_tracepoints             = NULL,
  .get_thread_area                  = NULL,
  .install_fast_tracepoint_jump_pad = NULL,
  .emit_ops                         = NULL,
  .get_min_fast_tracepoint_insn_len = NULL
};

void
initialize_low_arch (void)
{
  init_registers_arc ();

  initialize_regsets_info (&arc_regsets_info);
}
