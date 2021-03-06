/*
 * Copyright 2002-2004 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */

/*
 * @test
 * @bug 4496290 4985072
 * @summary A simple test to determine if -use works.
 * @author jamieh
 * @library ../lib/
 * @build JavadocTester
 * @build TestUseOption
 * @run main TestUseOption
 */

public class TestUseOption extends JavadocTester {

    private static final String BUG_ID = "4496290-4985072";

    //Input for string search tests.
    private static final String[] TEST2 = {
        "Field in C1.",
        "Field in C2.",
        "Field in C4.",
        "Field in C5.",
        "Field in C6.",
        "Field in C7.",
        "Field in C8.",
        "Method in C1.",
        "Method in C2.",
        "Method in C4.",
        "Method in C5.",
        "Method in C6.",
        "Method in C7.",
        "Method in C8.",
    };

    private static final String[] ARGS = new String[] {
        "-d", BUG_ID, "-sourcepath", SRC_DIR, "-use", "pkg1", "pkg2"
    };

    private static final String[] ARGS2 = new String[] {
        "-d", BUG_ID+"-2", "-sourcepath", SRC_DIR, "-use", "pkg1", "pkg2"
    };

    /**
     * The entry point of the test.
     * @param args the array of command line arguments.
     */
    public static void main(String[] args) throws Exception {
        String[][] tests = new String[11][2];
        //Eight tests for class use.
        for (int i = 0; i < 8; i++) {
            tests[i][0] = BUG_ID + FS + "pkg1" + FS + "class-use" + FS + "C1.html";
            tests[i][1] = "Test " + (i + 1) + " passes";
        }
        //Three more tests for package use.
        for (int i = 8, j = 1; i < tests.length; i++, j++) {
            tests[i][0] = BUG_ID + FS + "pkg1" + FS + "package-use.html";
            tests[i][1] = "Test " + j + " passes";
        }
        TestUseOption tester = new TestUseOption();
        run(tester, ARGS, tests, NO_TEST);
        tester.printSummary();
        run(tester, ARGS2, NO_TEST, NO_TEST);
        String usePageContents = tester.readFileToString(BUG_ID +"-2" + FS + "pkg1" + FS + "class-use" + FS + "UsedClass.html");
        int prevIndex = -1;
        int currentIndex = -1;
        for (int i = 0; i < TEST2.length; i++) {
            currentIndex = usePageContents.indexOf(TEST2[i]);
            System.err.println(TEST2[i] + " at index " + currentIndex);
            if (currentIndex < prevIndex)
                throw new Exception(TEST2[i] + " is in the wrong order.");
            prevIndex = currentIndex;
        }
        tester.printSummary();
    }

    /**
     * {@inheritDoc}
     */
    public String getBugId() {
        return BUG_ID;
    }

    /**
     * {@inheritDoc}
     */
    public String getBugName() {
        return getClass().getName();
    }
}
