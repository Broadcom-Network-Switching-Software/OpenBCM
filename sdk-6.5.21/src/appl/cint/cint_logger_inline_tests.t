/*
 * $Id: cint_logger_inline_tests.t
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_logger_inline_tests.t
 * Purpose:     CINT logger tests template.
 *
 * <DNE>
 *
 * <SKIPLINES>
 *
 * This file provides a template CINT for testing logger functionality in the
 * context of any API. The template contains Tokens which can be expanded to
 * API-specific text such as the API name, or API arguments etc later by a test
 * generator.
 *
 * This file is passed through a script $SDK/src/appl/cin/cint_logger_tests.pl
 * which converts it into compilable C code containing the template as a
 * character array, and a database of token addresses within that array.
 * Typically this converted output will be saved to an include file and
 * #include'd by the CINT logger test generator infrastructure.
 *
 * Logger test generator expands all tokens uniquely depending on which API is
 * being tested. The CINT generated after expanding all the tokens is passed to
 * the CINT interpreter which results in testing the logger in the context of
 * the given API.
 *
 * The template below has inline comments detailing the logic of the test. This
 * two-step approach (template -> test CINT) helps to separate the CINT
 * containing the test logic, and C code which supports the testing, into
 * different files.
 *
 * </SKIPLINES>
 */

static const char inline_test_templ[] = {

<hexdump>

struct cint_logger_test_data_t {
    int unit;
    int result;
    int test_tid_hi;
    int test_tid_lo;
    int test_call_id;
    /*
     * region for inline tests
     */
    int before_called;
    int after_called;
    /* declare place-holder for arg_ptrs */
    @decl_arg_ptrs@
    /*
     * region for offline tests
     */
    int before_found;
    int after_found;
};

/*
 * The variable "this" should exist auto-magically. Test that by referring to
 * this.flags without prototype.
 */
int CINT_LOGGER_IS_BEFORE ()
{
    uint32 flags = this.flags;

    return (((flags & 0x100) == 0x100) &&
            ((flags & 0x100) == flags));
}

/*
 * The variable "this" should exist auto-magically. Test that by referring to
 * this.flags without prototype.
 */
int CINT_LOGGER_IS_AFTER ()
{
    uint32 flags = this.flags;

    return (((flags & 0x200) == 0x200) &&
            ((flags & 0x200) == flags));
}

int PTR_HI_TO_INT (void* ptr)
{
    if (sizeof(ptr) == 8) {
        uint64 *u64 = (void*) &ptr;
        return COMPILER_64_HI(*u64);
    } else {
        return 0;
    }

}

int PTR_LO_TO_INT (void* ptr)
{
    if (sizeof(ptr) == 8) {
        uint64 *u64 = (void*) &ptr;
        return COMPILER_64_LO(*u64);
    } else {
        uint32 *u32 = (void*) &ptr;
        return *u32;
    }

}

cint_logger_test_data_t @api@_test;

int cint_logger_test_inline_verifier_@api@ ()
{
    /*
     * declare the arguments expected in the BEFORE call
     */
    printf("VERIFIER: declare the arguments expected in the BEFORE call\n");

    @decl_args_expected_before@

    /*
     * declare the arguments expected in the AFTER  call
     */
    printf("VERIFIER: declare the arguments expected in the AFTER  call\n");

    @decl_args_expected_after@

    /*
     * declare the variables holding changed values
     */
    printf("VERIFIER: declare the variables holding changed values\n");

    @decl_args_changes@

    /*
     * declare local variables we will need
     */
    printf("VERIFIER: declare local variables we will need\n");

    @decl_local_variables@

    /*
     * Verifier logic follows...
     */
    printf("VERIFIER: Verifier logic follows...\n");

    if (!cint_defined("this")) {
        /*
         * We should always be called in the context of the logger
         */
        printf("VERIFIER: We should always be called in the context of the logger...return\n");
        return 0;
    }

    if ((PTR_LO_TO_INT(this.caller_tid) != @api@_test.test_tid_lo) ||
        (PTR_HI_TO_INT(this.caller_tid) != @api@_test.test_tid_hi)) {
        /*
         * got called from another thread, do not interfere. This proves that
         * the application is multi-threaded.
         */
        printf("VERIFIER: got called from another thread %p, do not interfere...return\n", this.caller_tid);
        return 0;
    }

    /*
     * called in the logger context, and from the test thread. now we are
     * really testing...
     */
    @test_item@

    /*
     * @test_id@. expected to be called in the context of @api@ only
     */
    printf("VERIFIER(@test_id@): expected to be called in the context of @api@ only\n");
    if (sal_strcmp(this.api, "@api@") != 0) {

        printf("TEST FAILED(@test_id@): Expected \"@api@\" in this thread, found \"%s\" instead\n",
               this.api);
        @api@_test.result = @test_id@;
        return 1;
    }

    /*
     * Since this is only a test with random and likely invalid inputs, we do
     * not want the API to be dispatched. However, even if it was it makes no
     * difference to this test so long as the dispatch routine handles all the
     * garbage inputs gracefully and returns.
     */
    printf("VERIFIER: skip_dispatch=1, since this is only a test.\n");
    *this.skip_dispatch = 1;

    @test_item@;
    /*
     * @test_id@. sanitize the flags
     */
    printf("VERIFIER(@test_id@): sanitize this.flags\n");
    if (!CINT_LOGGER_IS_BEFORE() &&
        !CINT_LOGGER_IS_AFTER()) {

        printf("TEST FAILED(@test_id@): flags are not right 0x%x\n",
               this.flags);
        @api@_test.result = @test_id@;
    }

    @test_item@
    /*
     * @test_id@. since this is an inline test, auto-injected variables must
     * map to the same location as original arguments
     */
    printf("VERIFIER(@test_id@): check addresses of auto-injected variables, and pointer-type variables\n");

    @check_arg_ptrs@


    /*
     * get the arguments as would be expected BEFORE dispatch
     */
    printf("VERIFIER: get the arguments as would be expected BEFORE dispatch\n");

    @init_args_expected_before@

    /*
     * get the arguments as would be expected AFTER dispatch
     */
    printf("VERIFIER: get the arguments as would be expected AFTER dispatch\n");

    @init_args_expected_after@


    /*
     * Arguments fidelity checks!
     */
    if (CINT_LOGGER_IS_BEFORE()) {

        /*
         * Record that the BEFORE call was placed...
         */
        printf("VERIFIER: Record that the BEFORE call was placed...\n");

        @api@_test.before_called = 1;

        @test_item@
        /*
         * @test_id@. call_id must be greater than initial test_call_id. It
         * cannot be strictly +1 because application may be multi-threaded and
         * other APIs may be getting called
         */
        printf("VERIFIER(@test_id@): call_id must be greater than initial test_call_id.\n");
        if (this.call_id <= @api@_test.test_call_id) {
            printf("TEST FAILED(@test_id@): Expected call_id to be greater than %d, found %d instead\n",
                   @api@_test.test_call_id, this.call_id);
            @api@_test.result = @test_id@;
        }

        /*
         * save this call_id - we must find same call_id in the AFTER call also
         */
        printf("VERIFIER: save this call_id - we must find same call_id in the AFTER call also\n");

        @api@_test.test_call_id = this.call_id;


        @test_item@;
        /*
         * @test_id@. check whether arguments BEFORE are as expected
         */
        printf("VERIFIER(@test_id@): check whether arguments BEFORE are as expected\n");

        @args_check_before@

        if (@args_check_before_res@ != 0) {

            printf("TEST FAILED(@test_id@): BEFORE: arguments failed consistency check %d\n",
                   @args_check_before_res@);
            @api@_test.result = @test_id@;
        }

        @args_changes@

    } else if (CINT_LOGGER_IS_AFTER()) {

        /*
         * Record that the AFTER called was placed...
         */
        printf("VERIFIER: Record that the AFTER called was placed...\n");

        @api@_test.after_called = 1;

        @test_item@;
        /*
         * @test_id@. call_id must be same as the one recorded during BEFORE call
         */
        printf("VERIFIER(@test_id@): call_id must be same as the one recorded during BEFORE call\n");
        if (this.call_id != @api@_test.test_call_id) {

            printf("TEST FAILED(@test_id@): Expected call_id %d recording during BEFORE call, found %d instead\n",
                   @api@_test.test_call_id);
            @api@_test.result = @test_id@;
        }


        @test_item@;
        /*
         * @test_id@. check whether arguments are as expected
         */
        printf("VERIFIER(@test_id@): check whether arguments AFTER are as expected\n");

        @args_check_after@

        if (@args_check_after_res@ != 0) {

            printf("TEST FAILED(@test_id@): AFTER: arguments failed consistency check %d\n",
                   @args_check_after_incl_changes_res@);
            @api@_test.result = @test_id@;
        }

        printf("VERIFIER: assign the return value\n");

        @retval@

    } else {

        @test_item@;
        /*
         * @test_id@. This should actually never happen
         */
         printf("TEST FAILED(@test_id@): neither BEFORE nor AFTER call\n");
         @api@_test.result = @test_id@;
    }

    return 0;
}

int cint_logger_offline_verifier_@api@ ()
{
    @test_item@
    /*
     * @test_id@: Context variable "this" must exist
     */
    if (!cint_defined("this")) {

        printf("TEST FAILED(@test_id@): Context variable \"this\" must exist...return\n");
        @api@_test.result = @test_id@;
        return 1;
    }

    /*
     * since a walk over multiple entries is performed to search for the entries
     * of interest, quietly ignore the entries where call_id does not match.
     */
    if (this.call_id != @api@_test.test_call_id) {

        /*printf("found entry with call_id %d belonging to API %s, flags 0x%x - skipping\n", this.call_id, this.api, this.flags);*/
        return 1;
    }

    /*
     * found an entry with matching call_id. Now we are really testing.
     */
    printf("VERIFIER: found entry matching expected call_id %d\n", this.call_id);

    /*
     * declare the arguments expected in the BEFORE call
     */
    printf("VERIFIER: declare the arguments expected in the BEFORE call\n");

    @decl_args_expected_before@

    /*
     * declare the arguments expected in the AFTER  call
     */
    printf("VERIFIER: declare the arguments expected in the AFTER  call\n");

    @decl_args_expected_after@

    /*
     * declare the variables holding changed values
     */
    printf("VERIFIER: declare the variables holding changed values\n");

    @decl_args_changes@

    /*
     * declare local variables we will need
     */
    printf("VERIFIER: declare local variables we will need\n");

    @decl_local_variables@

    @test_item@
    /*
     * @test_id@. Thread ID must be the one we expected
     */
    if ((PTR_LO_TO_INT(this.caller_tid) != @api@_test.test_tid_lo) ||
        (PTR_HI_TO_INT(this.caller_tid) != @api@_test.test_tid_hi)) {
        /*
         * caller thread ID was incorrectly recorded
         */
        printf("VERIFIER(@test_id@): caller thread ID was incorrectly recorded (%p vs. 0x{%08x,%08x} expected)\n",
               this.caller_tid, @api@_test.test_tid_hi, @api@_test.test_tid_lo);
        return 1;
    }

    @test_item@
    /*
     * @test_id@. expected this entry to be for @api@ only
     */
    printf("VERIFIER(@test_id@): expected entry with call_id %d should be for @api@ only\n", this.call_id);
    if (sal_strcmp(this.api, "@api@") != 0) {

        printf("TEST FAILED(@test_id@): Expected \"@api@\" in this entry, found \"%s\" instead\n",
               this.api);
        @api@_test.result = @test_id@;
        return 1;
    }

    @test_item@
    /*
     * @test_id@. sanitize the flags
     */
    printf("VERIFIER(@test_id@): sanitize this.flags\n");
    if (!CINT_LOGGER_IS_BEFORE() &&
        !CINT_LOGGER_IS_AFTER()) {

        printf("TEST FAILED(@test_id@): flags are not right 0x%x\n",
               this.flags);
        @api@_test.result = @test_id@;
    }

    /*
     * get the arguments as would be expected BEFORE dispatch
     */
    printf("VERIFIER: get the arguments as would be expected in the BEFORE entry\n");

    @init_args_expected_before@

    /*
     * get the arguments as would be expected AFTER dispatch
     */
    printf("VERIFIER: get the arguments as would be in the AFTER entry\n");

    @init_args_expected_after@

    /*
     * Arguments fidelity checks!
     */
    if (CINT_LOGGER_IS_BEFORE()) {

        @test_item@
        /*
         * @test_id@. Check if BEFORE entry was previously found...suggesting duplicates
         */
        printf("VERIFIER: check if BEFORE entry was previously found...suggesting duplicates\n");
        if (@api@_test.before_found) {
            printf("TEST FAILED(@test_id@): BEFORE entry was already found\n");
            @api@_test.result = @test_id@;
        }

        /*
         * Record that the BEFORE entry was found...
         */
        printf("VERIFIER: Record that the BEFORE entry was found...\n");

        @api@_test.before_found = 1;

        @test_item@;
        /*
         * @test_id@. check whether arguments in BEFORE entry are as expected
         */
        printf("VERIFIER(@test_id@): check whether arguments in BEFORE entry are as expected\n");

        @args_check_before@

        if (@args_check_before_res@ != 0) {

            printf("TEST FAILED(@test_id@): BEFORE: arguments failed consistency check %d\n",
                   @args_check_before_res@);
            @api@_test.result = @test_id@;
        }

        @args_changes@

    } else if (CINT_LOGGER_IS_AFTER()) {

        @test_item@
        /*
         * @test_id@. Check if AFTER entry was previously found...suggesting duplicates
         */
        printf("VERIFIER: check if AFTER entry was previously found...suggesting duplicates\n");
        if (@api@_test.after_found) {
            printf("TEST FAILED(@test_id@): AFTER entry was already found\n");
            @api@_test.result = @test_id@;
        }

        /*
         * Record that the AFTER entry was found...
         */
        printf("VERIFIER: Record that the AFTER entry was found...\n");

        @api@_test.after_found = 1;

        @test_item@;
        /*
         * @test_id@. check whether arguments are as expected
         */
        printf("VERIFIER(@test_id@): check whether arguments AFTER are as expected\n");

        @args_check_after@

        if (@args_check_after_res@ != 0) {

            printf("TEST FAILED(@test_id@): AFTER: arguments failed consistency check %d\n",
                   @args_check_after_incl_changes_res@);
            @api@_test.result = @test_id@;
        }

    } else {

        @test_item@;
        /*
         * @test_id@. This should actually never happen
         */
         printf("TEST FAILED(@test_id@): neither BEFORE nor AFTER entry. Cannot process.\n");
         @api@_test.result = @test_id@;
    }

    return 1;
}

int cint_logger_test_@api@()
{
    int unit = @unit@;
    int enabled_save;
    int mode, mode_save;
    char cmd[256];

    /*
     * declare the API inputs
     */
    printf("TESTFUNC: declare the API inputs\n");

    @decl_args@

    /*
     * declare local variables we need
     */
    printf("TESTFUNC: declare local variables we need\n");

    @decl_local_variables@

    /*
     * Initialize the API inputs with their values
     */
    printf("TESTFUNC: Initialize the API inputs with their values\n");

    @init_args@

    /*
     * set up the test results structure
     */
    printf("TESTFUNC: set up the test results structure\n");
    @api@_test.unit = unit;
    @api@_test.result = 0;
    @api@_test.test_tid_lo = @test_tid_lo@;
    @api@_test.test_tid_hi = @test_tid_hi@;
    @api@_test.test_call_id = cint_logger_cfg.count;
    @api@_test.before_called = 0;
    @api@_test.after_called = 0;
    @api@_test.before_found = 0;
    @api@_test.after_found = 0;
    @set_arg_ptrs@

    /*
     * show the test results structure
     */
    print @api@_test;

    /*
     * Install the inline verifier. Save current mode and enable/disable status
     * of the logger, and then enable the logger with mode set to Log
     */
    printf("TESTFUNC: Install the verifier, enable Logger and set mode to Log\n");
    enabled_save = cint_logger_cfg.enabled;
    mode_save    = cint_logger_cfg.mode;
    mode         = cintLoggerModeLog;
    snprintf(cmd, sizeof(cmd), "logger cfg en=1 mode=%d fn=%s",
             mode, "cint_logger_test_inline_verifier_@api@");
    printf("        : Executing \"%s\"\n", cmd);
    bshell(unit, cmd);

    /*
     * Call the API !!!!
     */
    printf("TESTFUNC: !!!! Call the API !!!!\n");
    printf("@api_call@\n");

    @api_call@

    /*
     * Uninstall the verifier and Restore the previous configuration...
     */
    printf("TESTFUNC: Uninstall verifier, Restore logger configuration mode and enabled\n");
    snprintf(cmd, sizeof(cmd), "logger cfg en=%d mode=%d fn=\"\"", enabled_save, mode_save);
    printf("        : Executing \"%s\"\n", cmd);
    bshell(unit, cmd);


    @test_item@
    /*
     * @test_id@. check whether BEFORE call was received
     */
    printf("TESTFUNC(@test_id@): check whether BEFORE call was received\n");
    if (@api@_test.before_called == 0) {
        printf("TEST FAILED(@test_id@,@api@): the BEFORE call was not received\n");
        @api@_test.result = @test_id@;
    }

    @test_item@
    /*
     * @test_id@. check whether AFTER call was received
     */
    printf("TESTFUNC(@test_id@): check whether AFTER call was received\n");
    if (@api@_test.after_called == 0) {
        printf("TEST FAILED(@test_id@,@api@): the AFTER call was not received\n");
        @api@_test.result = @test_id@;
    }

    @test_item@
    /*
     * @test_id@. check the return value
     */
    printf("TESTFUNC(@test_id@): check the return value\n");

    @retval_check@



    /*
     * Now run the offline verification... run the show command with filter/
     * formatter routine. We expect to encounter the two entries created above
     * in this one loop.
     */
    printf("TESTFUNC: Now running offline tests\n");

    int num_entries = cint_logger_cfg.count * 2;
    snprintf(cmd, sizeof(cmd), "logger show fn=%d fmt=%s", num_entries, "cint_logger_offline_verifier_@api@");
    printf("TESTFUNC: running offline verifier over all entries\n");
    printf("        : Executing \"%s\"\n", cmd);
    bshell(unit, cmd);

    @test_item@
    /*
     * @test_id@. BEFORE entry should have been found
     */
    printf("TESTFUNC(@test_id@): BEFORE entry should have been found\n");
    if (@api@_test.before_found == 0) {
        printf("TEST FAILED(@test_id@,@api@): the BEFORE call entry was not found\n");
        @api@_test.result = @test_id@;
    }

    @test_item@
    /*
     * @test_id@. After entry should have been found
     */
    printf("TESTFUNC(@test_id@): AFTER entry should have been found\n");
    if (@api@_test.after_found == 0) {
        printf("TEST FAILED(@test_id@,@api@): the AFTER call entry was not found\n");
        @api@_test.result = @test_id@;
    }

    return (@api@_test.result);
}

/*
 * call the test function. This must be last because its return value signifies
 * whether the test-case passed or failed.
 */
cint_logger_test_@api@();

</hexdump>

    /*
     * mandatory NULL termination of the string
     */
    0x00,
};

/*
 * Tokens
 */
static cint_logger_tests_token_t inline_test_templ_tok_offsets[] = {

    {     0,     0,     0, CINT_LOGGER_TESTS_TOKEN_NONE },
<token_offsets>
    {    -1,    -1,    -1, CINT_LOGGER_TESTS_TOKEN_NONE },
};

