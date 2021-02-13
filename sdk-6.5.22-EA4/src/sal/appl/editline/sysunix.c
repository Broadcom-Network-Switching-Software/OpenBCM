/*  $Revision: 1.5 $
**
**  Unix system-dependant routines for editline library.
*/

#if defined(SYS_UNIX) && defined(INCLUDE_EDITLINE)

#include "editline.h"

/*
 Controls if changes need to ocur. . If we've already transitioned into the 
 requested mode do nothing. This allows for synchronous and asynchronous
 modes to work.

 sal_readline, synchronous editline, makes sure to put the tty back into
 cooked mode prior to exit. Asynchronous mode need to keep the tty in raw
 mode until an asynchronous exit from the shell.

 Values are:
  -1 Default initialize value which can not be 0 or 1. This is neccessary so 
     that when Reset is called the first time with either 0 or 1 that 
     actions will be taken.
  0 put device into raw mode. Disable echo so that editline can control it.
  1 restore device back to mode prior to raw mode.
*/
static int sRESET_CURRENT = -1;

#if	defined(HAVE_TCGETATTR)
#include <termios.h>

void
rl_ttyset(int Reset)
{
    static struct termios old;
    struct termios		  new;

    if (Reset != sRESET_CURRENT) {
      sRESET_CURRENT = Reset;
      if (Reset == 0) {
	    if (tcgetattr(0, &old) < 0) {
          perror("tcgetattr");
        }
	    rl_erase = old.c_cc[VERASE];
	    rl_kill = old.c_cc[VKILL];
	    rl_eof = old.c_cc[VEOF];
	    rl_intr = old.c_cc[VINTR];
	    rl_quit = old.c_cc[VQUIT];
#if	defined(DO_SIGTSTP)
	    rl_susp = old.c_cc[VSUSP];
#endif	/* defined(DO_SIGTSTP) */

	    new = old;
#if 0
	    new.c_lflag &= ~(ECHO | ICANON | ISIG);
#else
	    new.c_lflag &= ~(ECHO | ICANON);
#endif
	    new.c_iflag &= ~(ISTRIP | INPCK);
	    new.c_cc[VMIN] = 1;
	    new.c_cc[VTIME] = 0;
	    if (tcsetattr(0, TCSADRAIN, &new) < 0) {
          perror("tcsetattr");
        }
      } else {
        /* Reset must be 1. */
	    (void)tcsetattr(0, TCSADRAIN, &old);
      }
    }
}

#else
#if	defined(HAVE_TERMIO)
#include <termio.h>

void
rl_ttyset(int Reset)
{
    static struct termio old;
    struct termio		 new;

    if (Reset != sRESET_CURRENT) {
      sRESET_CURRENT = Reset;
      if (Reset == 0) {
        (void)ioctl(0, TCGETA, &old);
	    rl_erase = old.c_cc[VERASE];
	    rl_kill = old.c_cc[VKILL];
	    rl_eof = old.c_cc[VEOF];
	    rl_intr = old.c_cc[VINTR];
	    rl_quit = old.c_cc[VQUIT];
#if	defined(DO_SIGTSTP)
	    rl_susp = old.c_cc[VSUSP];
#endif	/* defined(DO_SIGTSTP) */

	    new = old;
#if 0
	    new.c_lflag &= ~(ECHO | ICANON | ISIG);
#else
	    new.c_lflag &= ~(ECHO | ICANON);
#endif
	    new.c_iflag &= ~(ISTRIP | INPCK);
	    new.c_cc[VMIN] = 1;
	    new.c_cc[VTIME] = 0;
	    (void)ioctl(0, TCSETAW, &new);
      } else {
        (void)ioctl(0, TCSETAW, &old);
      }
    }
}

#else
#include <sgtty.h>

void
rl_ttyset(int Reset)
{
    static struct sgttyb	old_sgttyb;
    static struct tchars	old_tchars;
    struct sgttyb		new_sgttyb;
    struct tchars		new_tchars;
#if	defined(DO_SIGTSTP)
    struct ltchars		old_ltchars;
#endif	/* defined(DO_SIGTSTP) */

    if (Reset != sRESET_CURRENT) {
      sRESET_CURRENT = Reset;
      if (Reset == 0) {
	    (void)ioctl(0, TIOCGETP, &old_sgttyb);
	    rl_erase = old_sgttyb.sg_erase;
	    rl_kill = old_sgttyb.sg_kill;

	    (void)ioctl(0, TIOCGETC, &old_tchars);
	    rl_eof = old_tchars.t_eofc;
	    rl_intr = old_tchars.t_intrc;
	    rl_quit = old_tchars.t_quitc;

#if	defined(DO_SIGTSTP)
	    (void)ioctl(0, TIOCGLTC, &old_ltchars);
	    rl_susp = old_ltchars.t_suspc;
#endif	/* defined(DO_SIGTSTP) */

	    new_sgttyb = old_sgttyb;
	    new_sgttyb.sg_flags &= ~ECHO;
	    new_sgttyb.sg_flags |= RAW;
#if	defined(PASS8)
	    new_sgttyb.sg_flags |= PASS8;
#endif	/* defined(PASS8) */
	    (void)ioctl(0, TIOCSETP, &new_sgttyb);
  
	    new_tchars = old_tchars;
	    new_tchars.t_intrc = -1;
	    new_tchars.t_quitc = -1;
	  (void)ioctl(0, TIOCSETC, &new_tchars);
      } else { (void)ioctl(0, TIOCSETP, &old_sgttyb);
        (void)ioctl(0, TIOCSETC, &old_tchars);
      }
    }
}
#endif	/* defined(HAVE_TERMIO) */
#endif	/* defined(HAVE_TCGETATTR) */

void
rl_add_slash(char	*path, char	*p)
{
#if     defined(USE_POSIX_COMPLETION)
    struct stat	Sb;

    if (stat(path, &Sb) >= 0)
	(void)strcat(p, S_ISDIR(Sb.st_mode) ? "/" : " ");
#endif
}

#endif /* defined(SYS_UNIX) && defined(INCLUDE_EDITLINE) */

typedef int _bcm_diag_editline_sysunix_not_empty; /* Make ISO compilers happy. */
