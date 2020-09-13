package info.shibafu528.yukari.exvoice.miquire;

/**
 * {@link Miquire#loadAll} の実行結果。
 */
public class MiquireResult {
    private final String[] success;
    private final String[] failure;

    /*package*/ MiquireResult(String[] success, String[] failure) {
        this.success = success;
        this.failure = failure;
    }

    /**
     * 読み込みに成功したプラグイン名のリストを返します。
     * @return
     */
    public String[] getSuccess() {
        return success;
    }

    /**
     * 読み込みに失敗したプラグイン名のリストを返します。
     * @return
     */
    public String[] getFailure() {
        return failure;
    }
}
