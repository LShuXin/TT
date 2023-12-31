package com.mogujie.tt.ui.activity;

import android.content.Intent;
import android.os.Bundle;
import android.view.Window;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;

import com.mogujie.tt.R;
import com.mogujie.tt.config.IntentConstant;
import com.mogujie.tt.imservice.event.LoginEvent;
import com.mogujie.tt.imservice.event.UnreadEvent;
import com.mogujie.tt.imservice.service.IMService;
import com.mogujie.tt.imservice.support.IMServiceConnector;
import com.mogujie.tt.ui.fragment.ChatFragment;
import com.mogujie.tt.ui.fragment.ContactFragment;
import com.mogujie.tt.ui.widget.NaviTabButton;
import com.mogujie.tt.utils.Logger;

import java.util.Objects;

import de.greenrobot.event.EventBus;


public class MainActivity extends FragmentActivity {
    private Fragment[] mFragments;
    private NaviTabButton[] mTabButtons;
    private final Logger logger = Logger.getLogger(MainActivity.class);
    private IMService imService;
    private final IMServiceConnector imServiceConnector = new IMServiceConnector() {
        @Override
        public void onIMServiceConnected() {
            imService = imServiceConnector.getIMService();
        }

        @Override
        public void onServiceDisconnected() {
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        logger.d("main_activity#onCreate savedInstanceState: %s", savedInstanceState);
        // when crash, this will be called, why?
        if (savedInstanceState != null) {
            logger.w("main_activity#onCreate crashed and restarted, just exit");
            jumpToLoginPage();
            finish();
        }

        // 在这个地方加可能会有问题吧
        EventBus.getDefault().register(this);
        imServiceConnector.connect(this);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.tt_activity_main);

        initTab();
        initFragment();
        setFragmentIndicator(0);
    }

    @Override
    public void onBackPressed() {
        //don't let it exit
        //super.onBackPressed();

        //nonRoot	If false then this only works if the activity is the root of a task; if true it will work for any activity in a task.
        //document http://developer.android.com/reference/android/app/Activity.html

        //moveTaskToBack(true);

        Intent i = new Intent(Intent.ACTION_MAIN);
        i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        i.addCategory(Intent.CATEGORY_HOME);
        startActivity(i);
    }

    // onNewIntent 是在一个活动（Activity）已经存在并处于活动状态（非销毁状态）时，
    // 通过 startActivity 启动该活动的时候调用的。它允许你处理新的 Intent，通常用
    // 于更新活动的内容或执行其他相关操作
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        handleLocateDepartment(intent);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        logger.d("main_activity#onDestroy");
        EventBus.getDefault().unregister(this);
        imServiceConnector.disconnect(this);
        super.onDestroy();
    }

    private void initFragment() {
        mFragments = new Fragment[4];
        mFragments[0] = getSupportFragmentManager().findFragmentById(R.id.fragment_chat);
        mFragments[1] = getSupportFragmentManager().findFragmentById(R.id.fragment_contact);
        mFragments[2] = getSupportFragmentManager().findFragmentById(R.id.fragment_internal);
        mFragments[3] = getSupportFragmentManager().findFragmentById(R.id.fragment_my);
    }

    private void initTab() {
        mTabButtons = new NaviTabButton[4];

        mTabButtons[0] = findViewById(R.id.tabbutton_chat);
        mTabButtons[1] = findViewById(R.id.tabbutton_contact);
        mTabButtons[2] = findViewById(R.id.tabbutton_internal);
        mTabButtons[3] = findViewById(R.id.tabbutton_my);

        mTabButtons[0].setTitle(getString(R.string.main_chat));
        mTabButtons[0].setIndex(0);
        mTabButtons[0].setSelectedImage(getResources().getDrawable(R.drawable.tt_tab_chat_sel));
        mTabButtons[0].setUnselectedImage(getResources().getDrawable(R.drawable.tt_tab_chat_nor));

        mTabButtons[1].setTitle(getString(R.string.main_contact));
        mTabButtons[1].setIndex(1);
        mTabButtons[1].setSelectedImage(getResources().getDrawable(R.drawable.tt_tab_contact_sel));
        mTabButtons[1].setUnselectedImage(getResources().getDrawable(R.drawable.tt_tab_contact_nor));

        mTabButtons[2].setTitle(getString(R.string.main_inner_net));
        mTabButtons[2].setIndex(2);
        mTabButtons[2].setSelectedImage(getResources().getDrawable(R.drawable.tt_tab_internal_select));
        mTabButtons[2].setUnselectedImage(getResources().getDrawable(R.drawable.tt_tab_internal_nor));

        mTabButtons[3].setTitle(getString(R.string.main_me_tab));
        mTabButtons[3].setIndex(3);
        mTabButtons[3].setSelectedImage(getResources().getDrawable(R.drawable.tt_tab_me_sel));
        mTabButtons[3].setUnselectedImage(getResources().getDrawable(R.drawable.tt_tab_me_nor));
    }

    public void setFragmentIndicator(int which) {
        getSupportFragmentManager()
                .beginTransaction()
                .hide(mFragments[0])
                .hide(mFragments[1])
                .hide(mFragments[2])
                .hide(mFragments[3])
                .show(mFragments[which])
                .commit();

        mTabButtons[0].setSelectedButton(false);
        mTabButtons[1].setSelectedButton(false);
        mTabButtons[2].setSelectedButton(false);
        mTabButtons[3].setSelectedButton(false);

        mTabButtons[which].setSelectedButton(true);
    }

    /** 会话 tab 未读消息数 */
    public void setUnreadMessageCnt(int unreadCnt) {
        mTabButtons[0].setUnreadNotify(unreadCnt);
    }

    /** 双击聊天 tab 时直接跳转到含有未读消息的会话位置 */
    public void chatDoubleListener() {
        setFragmentIndicator(0);
        ((ChatFragment) mFragments[0]).scrollToUnreadPosition();
    }

    /**
     * 如果传入了部门 ID，则直接跳转到联系人页面的对应部门处
     * */
    private void handleLocateDepartment(Intent intent) {
        int departmentIdToLocate = intent.getIntExtra(IntentConstant.KEY_LOCATE_DEPARTMENT, -1);
        if (departmentIdToLocate == -1) {
            return;
        }

        logger.d("main_activity#handleLocateDepartment department to locate: %d", departmentIdToLocate);
        setFragmentIndicator(1);
        ContactFragment fragment = (ContactFragment) mFragments[1];
        if (fragment == null) {
            logger.e("main_activity#handleLocateDepartment department fragment is null");
            return;
        }
        fragment.locateDepartment(departmentIdToLocate);
    }

    /** EventBus 事件处理 */
    public void onEventMainThread(UnreadEvent event) {
        switch (event.event) {
            case SESSION_READED_UNREAD_MSG:
            case UNREAD_MSG_LIST_OK:
            case UNREAD_MSG_RECEIVED:
                showUnreadMessageCount();
                break;
        }
    }

    /** 更新未读消息数量 */
    private void showUnreadMessageCount() {
        if (imService != null) {
            int unreadNum = imService.getUnReadMsgManager().getTotalUnreadCount();
            mTabButtons[0].setUnreadNotify(unreadNum);
        }
    }

    public void onEventMainThread(LoginEvent event) {
        if (Objects.requireNonNull(event) == LoginEvent.LOGIN_OUT) {
            handleOnLogout();
        }
    }

    private void handleOnLogout() {
        logger.d("main_activity#handleOnLogout");
        finish();
        logger.d("main_activity#handleOnLogout killed self, and start login activity");
        jumpToLoginPage();
    }

    private void jumpToLoginPage() {
        Intent intent = new Intent(this, LoginActivity.class);
        intent.putExtra(IntentConstant.KEY_LOGIN_NOT_AUTO, true);
        startActivity(intent);
    }
}
