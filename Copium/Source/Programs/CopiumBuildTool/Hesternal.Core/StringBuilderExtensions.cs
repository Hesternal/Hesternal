using System.Text;

namespace Hesternal.Core
{
    public static class StringBuilderExtensions
    {
        public static string ToStringAndClear(this StringBuilder stringBuilder)
        {
            string str = stringBuilder.ToString();
            stringBuilder.Clear();

            return str;
        }
    }
}
