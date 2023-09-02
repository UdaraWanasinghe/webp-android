package com.aureusapps.android.webpandroid.test.matchers

import org.hamcrest.BaseMatcher
import org.hamcrest.Description
import org.hamcrest.Matcher

fun inRange(start: Int, end: Int): Matcher<Int> {
    return InRangeMatcher(start, end)
}

private class InRangeMatcher(
    private val first: Int,
    private val last: Int
) : BaseMatcher<Int>() {

    override fun describeTo(description: Description) {
        description.appendText("value should be in range [$first, $last]")
    }

    override fun matches(item: Any?): Boolean {
        if (item is Int) {
            return item in first..last
        }
        return false
    }

    override fun describeMismatch(item: Any?, mismatchDescription: Description) {
        if (item !is Int) {
            mismatchDescription.appendText("value is not an integer")
            return
        }

        mismatchDescription.appendText("value is $item, which is not in range [$first, $last]")
    }

}